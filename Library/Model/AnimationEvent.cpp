#include "AnimationEvent.h"

#include <filesystem>
#include <fstream>
#include <imgui.h>

#include "../DebugSupporter/DebugSupporter.h"
#include "SerializeFunction.h"

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
        ImGui::DragFloat(u8"開始時間", &startSeconds, 0.01f);
        ImGui::DragFloat(u8"終了時間", &endSeconds, 0.01f);
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
        ImGui::DragFloat(u8"開始時間", &startSeconds, 0.01f);
        ImGui::DragFloat(u8"終了時間", &endSeconds, 0.01f);

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
    if (!Deserialize(_model.lock()->GetFilepath()))
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
void AnimationEvent::DebugRender(const std::string& animName, float animElapsedTime)
{
    if (_model.expired())
        return;

    for (auto& event : _data[animName.c_str()])
    {
        if (animElapsedTime > event.startSeconds &&
            animElapsedTime < event.endSeconds)
        {
            auto& node = _model.lock()->GetPoseNodes()[event.nodeIndex];
            DirectX::XMMATRIX T = DirectX::XMMatrixTranslationFromVector(DirectX::XMLoadFloat3(&event.position));
            DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(event.angle.x, event.angle.y, event.angle.z);
            DirectX::XMMATRIX S = DirectX::XMMatrixScalingFromVector(DirectX::XMLoadFloat3(&event.scale));
            DirectX::XMFLOAT4X4 transform = {};

            DirectX::XMStoreFloat4x4(&transform, S * R * T * DirectX::XMLoadFloat4x4(&node.worldTransform));

            switch (event.shapeType)
            {
            case AnimationEvent::ShapeType::Box:
                Debug::Renderer::DrawBox(transform, Vector4::White);
                break;
            case AnimationEvent::ShapeType::Sphere:
                Debug::Renderer::DrawSphere(Vector3(transform._41, transform._42, transform._43), event.scale.x, Vector4::White);
                break;
            case AnimationEvent::ShapeType::Capsule:
            {

                Debug::Renderer::DrawCapsule(
                    event.position.TransformCoord(node.worldTransform), 
                    event.angle.TransformCoord(node.worldTransform), 
                    event.scale.x,
                    Vector4::White);
            }
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
