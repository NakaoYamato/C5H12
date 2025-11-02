#include "AnimationEvent.h"

#include <filesystem>
#include <fstream>
#include <imgui.h>

#include "../DebugSupporter/DebugSupporter.h"
#include "SerializeFunction.h"

std::vector<int> s_startFrames;
std::vector<int> s_endFrames;

// データのバージョン管理
CEREAL_CLASS_VERSION(AnimationEvent, 0)

void AnimationEvent::EventData::DrawGui(const std::vector<const char*>& messageList, const std::vector<const char*>& nodeNames, bool canEdit)
{
    static const char* EventTypeNames[] =
    {
        u8"Flag",
        u8"Hit",
        u8"Attack",
    };
    static const char* ShapeTypeNames[] =
    {
        u8"Box",
        u8"Sphere",
        u8"Capsule",
    };

    // EventTypeの選択
    {
        int type = static_cast<int>(eventType);
        if (ImGui::Combo(u8"判定の種類", &type, EventTypeNames, _countof(EventTypeNames)))
            eventType = static_cast<EventType>(type);
    }
    switch (eventType)
    {
    case AnimationEvent::EventType::Flag:
        if (messageList.size() != 0)
			ImGui::Combo(u8"メッセージ", &messageIndex, messageList.data(), (int)messageList.size());
        //ImGui::DragFloat(u8"開始時間", &startSeconds, 0.01f);
        //ImGui::DragFloat(u8"終了時間", &endSeconds, 0.01f);
        ImGui::DragFloat3(u8"position", &position.x, 0.1f);
        break;
    case AnimationEvent::EventType::Hit:
    case AnimationEvent::EventType::Attack:
    {
        // ShapeTypeの選択
        {
            int type = static_cast<int>(shapeType);
            if (ImGui::Combo(u8"判定の形状", &type, ShapeTypeNames, _countof(ShapeTypeNames)))
                shapeType = static_cast<ShapeType>(type);
        }
        // nodeの選択
        ImGui::Combo(u8"ノード", &nodeIndex, nodeNames.data(), (int)nodeNames.size());
        if (messageList.size() != 0)
            ImGui::Combo(u8"ヒット時のメッセージ", &messageIndex, messageList.data(), (int)messageList.size());
        //ImGui::DragFloat(u8"開始時間", &startSeconds, 0.01f);
        //ImGui::DragFloat(u8"終了時間", &endSeconds, 0.01f);

        switch (shapeType)
        {
        case AnimationEvent::ShapeType::Box:
            ImGui::DragFloat3(u8"position", &position.x, 0.1f);
            ImGui::DragFloat3(u8"angle", &angle.x, 0.1f);
            ImGui::DragFloat3(u8"scale", &scale.x, 0.1f);
            break;
        case AnimationEvent::ShapeType::Sphere:
            ImGui::DragFloat3(u8"position", &position.x, 0.1f);
            ImGui::DragFloat(u8"radius", &scale.x, 0.1f);
            break;
        case AnimationEvent::ShapeType::Capsule:
            ImGui::DragFloat3(u8"start", &position.x, 0.1f);
            ImGui::DragFloat3(u8"end", &angle.x, 0.1f);
            ImGui::DragFloat(u8"radius", &scale.x, 0.1f);
            break;
        }
    }
        break;
    }
}

template<class T>
inline void AnimationEvent::EventData::serialize(T& archive, const std::uint32_t version)
{
    if (version == 0)
    {
        archive(
            CEREAL_NVP(eventType),
            CEREAL_NVP(shapeType),
            CEREAL_NVP(nodeIndex),
            CEREAL_NVP(messageIndex),
            CEREAL_NVP(startSeconds),
            CEREAL_NVP(endSeconds),
            CEREAL_NVP(position),
            CEREAL_NVP(angle),
            CEREAL_NVP(scale)
        );
    }
}

/// モデル情報読み込み
void AnimationEvent::Load(std::weak_ptr<Model> model)
{
	assert(model.lock() != nullptr);

    _model = model;

	// モデルのシリアライズパスからイベント情報のパスを取得
    if (!Deserialize(_model.lock()->GetFilename()))
    {
        // なかったら新規作成
        Clear();

        // アニメーション名を登録
        for (auto& animation : _model.lock()->GetResource()->GetAnimations())
        {
            AddEventData(animation.name);
        }
    }

    _nodeNames.clear();
    // ノードの名前を全取得
    for (auto& node : _model.lock()->GetPoseNodes())
    {
        _nodeNames.push_back(node.name.c_str());
    }
}

/// デバッグ表示
void AnimationEvent::DebugRender(const std::string& animName, float animElapsedTime, const DirectX::XMFLOAT4X4& world)
{
    if (_model.expired())
        return;

    for (auto& event : _data[animName.c_str()])
    {
        if (animElapsedTime > event.startSeconds &&
            animElapsedTime < event.endSeconds)
        {
            // フラグイベントは無視
            if (event.eventType == AnimationEvent::EventType::Flag)
                continue;

            auto& node = _model.lock()->GetPoseNodes()[event.nodeIndex];
            DirectX::XMMATRIX T = DirectX::XMMatrixTranslationFromVector(DirectX::XMLoadFloat3(&event.position));
            DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(event.angle.x, event.angle.y, event.angle.z);
            DirectX::XMMATRIX S = DirectX::XMMatrixScalingFromVector(DirectX::XMLoadFloat3(&event.scale));
            DirectX::XMFLOAT4X4 transform = {};
            DirectX::XMStoreFloat4x4(&transform, S * R * T * DirectX::XMLoadFloat4x4(&node.worldTransform));

            Vector4 color = Vector4::White;
            // イベントタイプによって色を変える
            switch (event.eventType)
            {
            case AnimationEvent::EventType::Hit:
                color = Vector4::Blue;
                break;
            case AnimationEvent::EventType::Attack:
                color = Vector4::Red;
                break;
            }

            switch (event.shapeType)
            {
            case AnimationEvent::ShapeType::Box:
                Debug::Renderer::DrawBox(transform, color);
                break;
            case AnimationEvent::ShapeType::Sphere:
                Debug::Renderer::DrawSphere(Vector3(transform._41, transform._42, transform._43), event.scale.x, color);
                break;
            case AnimationEvent::ShapeType::Capsule:
                Debug::Renderer::DrawCapsule(
                    event.position.TransformCoord(node.worldTransform),
                    event.angle.TransformCoord(node.worldTransform),
                    event.scale.x,
                    color);
                break;
            }
        }
    }
}

// GUI描画
void AnimationEvent::DrawGui(bool canEdit)
{
    if (_model.expired())
        return;

    for (auto& [animName, datas] : _data)
    {
        // アニメーション毎のキーフレームGUI
        if (ImGui::TreeNode(animName.c_str()))
        {
            DrawGui(animName, canEdit);
            ImGui::Separator();

            ImGui::TreePop();
        }
    }
}

// 指定したEventDataのGUI描画
void AnimationEvent::DrawGui(const std::string& animName, bool canEdit)
{
    if (_model.expired())
        return;

    auto& keyframes = _data[animName];

    if (canEdit)
    {
        if (ImGui::Button(u8"キーフレーム追加"))
        {
            keyframes.push_back(AnimationEvent::EventData());
        }
    }

    std::vector<const char*> messageList;
	messageList.reserve(_messageList.size());
	for (auto& message : _messageList)
	{
		messageList.push_back(message.c_str());
	}

    int index = 0;
    for (auto& keyframe : keyframes)
    {
        if (ImGui::TreeNode(std::to_string(index).c_str()))
        {
            // 各キーフレームGUI
            keyframe.DrawGui(messageList, _nodeNames, canEdit);

            if (canEdit)
            {
                if (ImGui::Button(u8"削除"))
                {
                    // 削除で配列が変更されているのでbreakで処理を強制終了させている
                    keyframes.erase(keyframes.begin() + index);
                    ImGui::TreePop();
                    break;
                }
            }

            ImGui::TreePop();
        }
        index++;
    }
}

void AnimationEvent::DrawGui(const std::string& animName, float currentAnimTime, bool canEdit)
{
    if (_model.expired())
        return;

    auto& keyframes = _data[animName];

    if (canEdit)
    {
        if (ImGui::Button(u8"キーフレーム追加"))
        {
            keyframes.push_back(AnimationEvent::EventData());
        }
    }

    // ... 既存の currentSelectedEntry 管理 (animationSelectedEntry) ...
    static std::unordered_map<std::string, int> animationSelectedEntry;
    int& currentSelectedEntry = animationSelectedEntry[animName];

    // シーケンサーオプションの設定
    int options = ImSequencer::SEQUENCER_EDIT_ALL;
    // 秒数をフレームに変換 (仮に60FPS)
    static const float FPS = 60.0f;
    static int currentFrame = 0; // currentFrameをstaticのまま使用
    static bool expanded = true;
    static int firstFrame = 0; // firstFrameをstaticのまま使用

    currentFrame = (int)(currentAnimTime * FPS);

    // Get()が呼ばれる前にフレームバッファを初期化
    s_startFrames.resize(keyframes.size());
    s_endFrames.resize(keyframes.size());
    for (size_t i = 0; i < keyframes.size(); ++i)
    {
        s_startFrames[i] = (int)(keyframes[i].startSeconds * FPS);
        s_endFrames[i] = (int)(keyframes[i].endSeconds * FPS);
    }// *** 修正: シーケンサーの縦幅を計算し、ImGuiのグループで確保 ***
    const float ItemHeight = 20.f; // ImSequencer.cpp内の ItemHeight = 20
    const float HeaderHeight = ItemHeight;
    const float ScrollBarHeight = 14.f; // ImSequencer.cpp内の scrollBarHeight = 14

    float totalCustomHeight = 0.f;
    for (size_t i = 0; i < keyframes.size(); ++i)
    {
        // 選択されたアイテムのカスタムハイトを取得
        bool isSelected = (int)i == currentSelectedEntry;
        if (canEdit && isSelected)
            totalCustomHeight += 120.f; // CustomDrawで設定した高さ
    }

    // 必要な総高さ = ヘッダー + (アイテム数 * アイテムの基本の高さ) + カスタムハイト + スクロールバー（仮に表示しないと仮定）
    float requiredHeight = HeaderHeight + (keyframes.size() * ItemHeight) + totalCustomHeight + ScrollBarHeight;

    // ImGui::BeginChild を使用して、必要な高さを確保
    ImGui::BeginChild("SequencerContainer", ImVec2(0, requiredHeight), true, ImGuiWindowFlags_NoScrollbar); // X方向は利用可能幅、Y方向は計算した高さ

    // シーケンサーインターフェースを実装したクラスのインスタンスを生成
    // *** 修正: s_startFrames, s_endFrames のアドレスを渡します ***
    AnimationSequencer sequencer(keyframes, _messageList, _nodeNames, canEdit, &currentSelectedEntry, &s_startFrames, &s_endFrames);


    // シーケンサーの描画
    ImGui::PushID(animName.c_str());
    ImSequencer::Sequencer(&sequencer, &currentFrame, &expanded, &currentSelectedEntry, &firstFrame, options);
    ImGui::PopID();

    ImGui::EndChild(); // 確保した領域の終了

    // シーケンサーの後に EventData の詳細 GUI を描画
    if (canEdit && currentSelectedEntry >= 0 && currentSelectedEntry < keyframes.size())
    {
        // メッセージリストの準備
        std::vector<const char*> messageList;
        messageList.reserve(_messageList.size());
        for (auto& message : _messageList)
        {
            messageList.push_back(message.c_str());
        }

        // 選択された EventData の DrawGui を呼び出す
        ImGui::PushID(currentSelectedEntry);
        ImGui::SetNextItemWidth(200);
        keyframes[currentSelectedEntry].DrawGui(messageList, _nodeNames, canEdit);
        if (ImGui::Button(u8"削除"))
        {
            // 削除で配列が変更されているのでbreakで処理を強制終了させている
            keyframes.erase(keyframes.begin() + currentSelectedEntry);
        }
        ImGui::PopID();

        ImGui::Unindent();
    }

    // シーケンサーで変更されたフレーム位置をEventDataの秒数に反映
    // Get()関数で渡した静的バッファから値を読み取って書き戻します
    for (size_t i = 0; i < keyframes.size(); ++i)
    {
        // フレーム -> float 秒数 に変換して EventData を更新
        keyframes[i].startSeconds = (float)s_startFrames[i] / FPS;
        keyframes[i].endSeconds = (float)s_endFrames[i] / FPS;
    }
}

/// メッセージリストの編集GUI描画
void AnimationEvent::DrawMassageListGui(bool canEdit)
{
    int index = 0;
    for (auto& message : _messageList)
    {
        if (canEdit)
        {
            ImGui::InputText(std::to_string(index).c_str(), &message);
            ImGui::SameLine();
            if (ImGui::Button((std::to_string(index) + u8"削除").c_str()))
            {
                _messageList.erase(_messageList.begin() + index);
                break;
            }
        }
        else
        {
            ImGui::Text((std::to_string(index) + ":").c_str());
			ImGui::SameLine();
			ImGui::Text(message.c_str());
        }
        index++;
    }

	if (canEdit)
	{
        if (ImGui::Button(u8"メッセージ追加"))
        {
            _messageList.push_back("");
        }
	}
}

#pragma region ファイル操作
/// データ書き出し
bool AnimationEvent::Serialize(const char* filename)
{
    std::filesystem::path serializePath(filename);
    serializePath.replace_extension(ANIMATION_EVENT_EXTENSION);

    std::ofstream ostream(serializePath.string().c_str(), std::ios::binary);
    if (ostream.is_open())
    {
        cereal::BinaryOutputArchive archive(ostream);

        try
        {
            archive(
                CEREAL_NVP(_messageList),
                CEREAL_NVP(_data)
            );
            return true;
        }
        catch (...)
        {
        }
    }
    return false;
}

/// データ読み込み
bool AnimationEvent::Deserialize(const char* filename)
{
    std::filesystem::path serializePath(filename);
    serializePath.replace_extension(ANIMATION_EVENT_EXTENSION);

    std::ifstream istream(serializePath.string().c_str(), std::ios::binary);
    if (istream.is_open())
    {
        cereal::BinaryInputArchive archive(istream);

        try
        {


            archive(
                CEREAL_NVP(_messageList),
                CEREAL_NVP(_data)
            );
            return true;
        }
        catch (...)
        {
        }
    }
    return false;
}
#pragma endregion

#pragma region アクセサ
/// アニメーション名と経過時間からイベントデータを取得
AnimationEvent::EventDataMap AnimationEvent::GetCurrentEventData(const std::string& animName, float animElapsedTime)
{
    EventDataMap result;
    for (auto& event : _data[animName])
    {
        if (event.startSeconds <= animElapsedTime && animElapsedTime <= event.endSeconds)
        {
            result.push_back(event);
        }
    }
    return result;
}
#pragma endregion

// AnimationSequencer の静的メンバの定義 (C++のEventTypeに対応)
const char* AnimationEvent::AnimationSequencer::EventTypeNames[] =
{
    u8"Flag",
    u8"Hit",
    u8"Attack",
};

// イベントタイプに対応する色を定義
unsigned int AnimationEvent::AnimationSequencer::EventTypeColors[] =
{
    0xFF00FF00, // Green for Flag
    0xFF0000FF, // Blue for Hit
    0xFFFF0000, // Red for Attack
};

AnimationEvent::AnimationSequencer::AnimationSequencer(EventDataMap& data,
    const std::vector<std::string>& messageList,
    const std::vector<const char*>& nodeNames,
    bool canEdit,
    int* selectedEntry,
    std::vector<int>* startFrames, // *** 追加 ***
    std::vector<int>* endFrames) // *** 追加 ***
    : m_data(data)
    , m_messageList(messageList)
    , m_nodeNames(nodeNames)
    , m_canEdit(canEdit)
    , m_selectedEntry(selectedEntry)
    , m_startFrames(startFrames) // *** 初期化 ***
    , m_endFrames(endFrames) // *** 初期化 ***
{
}

void AnimationEvent::AnimationSequencer::Get(int index, int** start, int** end, int* type, unsigned int* color)
{
    // フレームレートはここでは不要
    // static const float FPS = 60.0f; // 削除

    // Get()が呼ばれる前に m_startFrames/m_endFrames の初期化とリサイズが
    // DrawGuiで保証されているため、ここでは安全に参照できます。

    // *** 修正: メンバ変数 m_startFrames/m_endFrames を利用します ***
    if (m_startFrames && m_startFrames->size() > (size_t)index)
    {
        if (start)
            *start = &(*m_startFrames)[index];
        if (end)
            *end = &(*m_endFrames)[index];
    }
    // else の処理 (エラーハンドリング) は省略

    if (type)
    {
        auto& eventData = m_data[index];
        *type = (int)eventData.eventType;
    }

    if (color)
    {
        auto& eventData = m_data[index];
        *color = EventTypeColors[(int)eventData.eventType];
    }
}

const char* AnimationEvent::AnimationSequencer::GetItemLabel(int index) const
{
    // ラベルはイベントタイプ名とインデックスの組み合わせ
    static std::string label;
    label = std::to_string(index) + ": " + EventTypeNames[(int)m_data[index].eventType];
    return label.c_str();
}

int AnimationEvent::AnimationSequencer::GetItemTypeCount() const
{
    return (int)EventType::EventTypeEnd;
}

const char* AnimationEvent::AnimationSequencer::GetItemTypeName(int typeIndex) const
{
    return EventTypeNames[typeIndex];
}
