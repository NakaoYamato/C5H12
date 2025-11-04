#include "AnimationEvent.h"

#include <filesystem>
#include <fstream>

#include <imgui.h>
#include <imgui_internal.h>
#include <ImSequencer.h>

#include "../DebugSupporter/DebugSupporter.h"
#include "SerializeFunction.h"

std::vector<int> s_startFrames;
std::vector<int> s_endFrames;

// データのバージョン管理
CEREAL_CLASS_VERSION(AnimationEvent, 0)

namespace AnimationEventGuiHelper
{
    // ImSequencer::SequenceInterface の実装
    class AnimationSequencer : public ImSequencer::SequenceInterface
    {
    public:
        AnimationSequencer(AnimationEvent::EventDataList& data,
            const std::vector<std::string>& messageList,
            const std::vector<const char*>& nodeNames,
            bool canEdit,
            int* selectedEntry,
			int frameMax,
            std::vector<int>* startFrames,
            std::vector<int>* endFrames)
            : _data(data)
            , _messageList(messageList)
            , _nodeNames(nodeNames)
            , _canEdit(canEdit)
            , _selectedEntry(selectedEntry)
			, _frameMax(frameMax)
            , _startFrames(startFrames)
            , _endFrames(endFrames)
        {
        }

        virtual int GetFrameMin() const override { return 0; }
        virtual int GetFrameMax() const override { return _frameMax; }
        virtual int GetItemCount() const override { return (int)_data.size(); }

        virtual void Get(int index, int** start, int** end, int* type, unsigned int* color) override
        {
            if (_startFrames && _startFrames->size() > (size_t)index)
            {
                if (start)
                    *start = &(*_startFrames)[index];
                if (end)
                    *end = &(*_endFrames)[index];
            }
            // else の処理 (エラーハンドリング) は省略

            if (type)
            {
                auto& eventData = _data[index];
                *type = (int)eventData.eventType;
            }

            if (color)
            {
                auto& eventData = _data[index];
                *color = EventTypeColors[(int)eventData.eventType];
            }
        }

        virtual const char* GetItemLabel(int index) const override
        {
            // ラベルはイベントタイプ名とインデックスの組み合わせ
            static std::string label;
            switch (_data[index].eventType)
            {
			case AnimationEvent::EventType::Flag:
                if (_data[index].messageIndex >= 0 &&
                    _data[index].messageIndex < (int)_messageList.size())
                {
                    label = std::to_string(index) + ": " + _messageList[_data[index].messageIndex];
                }
                else
                {
					label = std::to_string(index) + ": " + EventTypeNames[(int)_data[index].eventType];
                }
                break;
            default:
                label = std::to_string(index) + ": " + EventTypeNames[(int)_data[index].eventType];
                break;
            }

            return label.c_str();
        }
        virtual int GetItemTypeCount() const override
        {
            return (int)AnimationEvent::EventType::EventTypeEnd;
        }
        virtual const char* GetItemTypeName(int typeIndex) const override
        {
            return EventTypeNames[typeIndex];
        }
    private:
        AnimationEvent::EventDataList& _data; // 参照としてイベントデータを持つ
        const std::vector<std::string>& _messageList;
        const std::vector<const char*>& _nodeNames;
		const int _frameMax = 10000;
        bool _canEdit;
        int* _selectedEntry;
        // DrawGuiから渡されるフレームバッファへのポインター
        std::vector<int>* _startFrames;
        std::vector<int>* _endFrames;
        // イベントタイプ名と色
        const char* EventTypeNames[3] =
        {
            u8"Flag",
            u8"Hit",
            u8"Attack",
        };
        unsigned int EventTypeColors[3] =
        {
            0xFF00FF00, // Green for Flag
            0xFF0000FF, // Blue for Hit
            0xFFFF0000, // Red for Attack
        };
    };
}

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
    
    if (messageList.size() != 0)
        ImGui::Combo(u8"メッセージ", &messageIndex, messageList.data(), (int)messageList.size());
    ImGui::DragFloat(u8"開始時間", &startSeconds, 0.01f, 0.0f, endSeconds);
    ImGui::DragFloat(u8"終了時間", &endSeconds, 0.01f, startSeconds);

    switch (eventType)
    {
    case AnimationEvent::EventType::Flag:
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

/// GUI描画
float AnimationEvent::DrawGui(const std::string& animName, float currentAnimTime, float endAnimTime, bool canEdit)
{
    static std::unordered_map<std::string, int> animationSelectedEntry;
    // シーケンサーオプションの設定
    static const int options = ImSequencer::SEQUENCER_EDIT_ALL;
    static const float FPS = 60.0f;// 秒数をフレームに変換
    static int currentFrame = 0; // currentFrameをstaticのまま使用
    static bool expanded = true;
    static int firstFrame = 0; // firstFrameをstaticのまま使用
    static const float ItemHeight = 20.f; // ImSequencer.cpp内の ItemHeight = 20
    static const float HeaderHeight = ItemHeight;
    static const float ScrollBarHeight = 14.f; // ImSequencer.cpp内の scrollBarHeight = 14

    if (_model.lock() == nullptr)
        return -1.0f;

    auto& keyframes = _data[animName];

    if (canEdit)
    {
        if (ImGui::Button(u8"キーフレーム追加"))
        {
            keyframes.push_back(AnimationEvent::EventData());
        }
    }

    int& currentSelectedEntry = animationSelectedEntry[animName];

    currentFrame = (int)(currentAnimTime * FPS);

    // Get()が呼ばれる前にフレームバッファを初期化
    s_startFrames.resize(keyframes.size());
    s_endFrames.resize(keyframes.size());
    for (size_t i = 0; i < keyframes.size(); ++i)
    {
        s_startFrames[i] = (int)(keyframes[i].startSeconds * FPS);
        s_endFrames[i] = (int)(keyframes[i].endSeconds * FPS);
    }

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
    AnimationEventGuiHelper::AnimationSequencer sequencer(
        keyframes,
        _messageList,
        _nodeNames,
        canEdit, 
        &currentSelectedEntry,
		(int)(FPS * endAnimTime),
        &s_startFrames,
        &s_endFrames);

    // シーケンサーの描画
    ImGui::PushID(animName.c_str());
    ImSequencer::Sequencer(&sequencer, &currentFrame, &expanded, &currentSelectedEntry, &firstFrame, options);
    ImGui::PopID();

    ImGui::EndChild(); // 確保した領域の終了

    // シーケンサーで変更されたフレーム位置をEventDataの秒数に反映
    // Get()関数で渡した静的バッファから値を読み取って書き戻します
    for (size_t i = 0; i < keyframes.size(); ++i)
    {
        // 開始フレームが終了フレームを超えないように、最小幅を1フレームとします。
        if (s_startFrames[i] > s_endFrames[i])
        {
            // 開始フレーム編集時: l=r となる
            // 終了フレーム編集時: r=l となる
            s_endFrames[i] = s_startFrames[i];
        }
        // フレーム -> float 秒数 に変換して EventData を更新
        keyframes[i].startSeconds = (float)s_startFrames[i] / FPS;
        keyframes[i].endSeconds = (float)s_endFrames[i] / FPS;
    }

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

    if (currentFrame != (int)(currentAnimTime * FPS))
		return static_cast<float>(currentFrame) / FPS;

    return -1.0f;
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
AnimationEvent::EventDataList AnimationEvent::GetCurrentEventData(const std::string& animName, float animElapsedTime)
{
    EventDataList result;
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
