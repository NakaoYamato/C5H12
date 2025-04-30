#include "AnimationCollisionData.h"

#include <filesystem>
#include <fstream>
#include <imgui.h>

#include "../../Library/Resource/SerializeFunction.h"

void AnimationCollisionData::Keyframe::DrawGui(const std::vector<const char*>& nodeNames)
{
    // CollisionTypeの選択
    {
        static const char* CollisionTypeNames[] =
        {
            u8"Hit",
            u8"Attack",
            u8"Other",
        };
        int type = static_cast<int>(collisionType);
        if (ImGui::Combo(u8"判定の種類", &type, CollisionTypeNames, _countof(CollisionTypeNames)))
            collisionType = static_cast<CollisionType>(type);
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
void AnimationCollisionData::Keyframe::serialize(T& archive)
{
    archive(
        CEREAL_NVP(collisionType),
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


// すべてのGUI描画
void AnimationCollisionData::DrawGuiAll(const std::vector<const char*>& nodeNames)
{
    for (auto& [animName, keyframes] : _data)
    {
        // アニメーション毎のキーフレームGUI
        if (ImGui::TreeNode(animName.c_str()))
        {
            DrawGui(animName, nodeNames);
            ImGui::Separator();

            ImGui::TreePop();
        }
    }
}

// 指定したKeyframesのGUI描画
void AnimationCollisionData::DrawGui(const std::string& animName, const std::vector<const char*>& nodeNames)
{
    auto& keyframes = _data[animName];
    if (ImGui::Button(u8"キーフレーム追加"))
    {
        keyframes.push_back(AnimationCollisionData::Keyframe());
    }

    int index = 0;
    for (auto& keyframe : keyframes)
    {
        if (ImGui::TreeNode(std::to_string(index).c_str()))
        {
            // 各キーフレームGUI
            keyframe.DrawGui(nodeNames);

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

// 書き出し
bool AnimationCollisionData::Serialize(const char* filename)
{
    std::filesystem::path serializePath(filename);
    serializePath.replace_extension(ANIMATION_COLLISION_EXTENSION);

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

bool AnimationCollisionData::Deserialize(const char* filename)
{
    std::filesystem::path serializePath(filename);
    serializePath.replace_extension(ANIMATION_COLLISION_EXTENSION);

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
