#include "AnimationEvent.h"

#include <filesystem>
#include <fstream>
#include <imgui.h>

#include "../DebugSupporter/DebugSupporter.h"
#include "SerializeFunction.h"

void AnimationEvent::EventData::DrawGui(const std::vector<const char*>& nodeNames)
{
    // EventTypeの選択
    {
        static const char* EventTypeNames[] =
        {
            u8"Flag",
            u8"Hit",
            u8"Attack",
        };
        int type = static_cast<int>(eventType);
        if (ImGui::Combo(u8"判定の種類", &type, EventTypeNames, _countof(EventTypeNames)))
            eventType = static_cast<EventType>(type);
    }
    // ShapeTypeの選択
    {
        static const char* ShapeTypeNames[] =
        {
            u8"Box",
            u8"Sphere",
            u8"Capsule",
        };
        int type = static_cast<int>(shapeType);
        if (ImGui::Combo(u8"判定の形状", &type, ShapeTypeNames, _countof(ShapeTypeNames)))
            shapeType = static_cast<ShapeType>(type);
    }
    // nodeの選択
    ImGui::Combo(u8"ノード", &nodeIndex, nodeNames.data(), (int)nodeNames.size());
    ImGui::InputText(u8"ヒット時のメッセージ", &triggerMessage);
    ImGui::DragFloat(u8"開始時間", &startSeconds, 0.01f);
    ImGui::DragFloat(u8"終了時間", &endSeconds, 0.01f);

    ImGui::DragFloat3(u8"position", &position.x, 0.1f);
    ImGui::DragFloat3(u8"angle", &angle.x, 0.1f);
    ImGui::DragFloat3(u8"scale", &scale.x, 0.1f);
}

template<class T>
inline void AnimationEvent::EventData::serialize(T& archive)
{
    archive(
        CEREAL_NVP(eventType),
        CEREAL_NVP(shapeType),
        CEREAL_NVP(nodeIndex),
        CEREAL_NVP(triggerMessage),
        CEREAL_NVP(startSeconds),
        CEREAL_NVP(endSeconds),
        CEREAL_NVP(position),
        CEREAL_NVP(angle),
        CEREAL_NVP(scale)
    );
}

/// モデル情報読み込み
void AnimationEvent::Load(std::weak_ptr<Model> model)
{
    _model = model;

    _nodeNames.clear();
    // ノードの名前を全取得
    for (auto& node : model.lock()->GetPoseNodes())
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
                Debug::Renderer::DrawCapsule(transform, 1.0f, 1.0f, Vector4::White);
                break;
            }
        }
    }
}

// GUI描画
void AnimationEvent::DrawGui()
{
    if (_model.expired())
        return;

    for (auto& [animName, datas] : _data)
    {
        // アニメーション毎のキーフレームGUI
        if (ImGui::TreeNode(animName.c_str()))
        {
            DrawGui(animName);
            ImGui::Separator();

            ImGui::TreePop();
        }
    }
}

// 指定したEventDataのGUI描画
void AnimationEvent::DrawGui(const std::string& animName)
{
    if (_model.expired())
        return;

    auto& keyframes = _data[animName];
    if (ImGui::Button(u8"キーフレーム追加"))
    {
        keyframes.push_back(AnimationEvent::EventData());
    }

    int index = 0;
    for (auto& keyframe : keyframes)
    {
        if (ImGui::TreeNode(std::to_string(index).c_str()))
        {
            // 各キーフレームGUI
            keyframe.DrawGui(_nodeNames);

            if (ImGui::Button(u8"削除"))
            {
                // 削除で配列が変更されているのでbreakで処理を強制終了させている
                keyframes.erase(keyframes.begin() + index);
                ImGui::TreePop();
                break;
            }

            ImGui::TreePop();
        }
        index++;
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
