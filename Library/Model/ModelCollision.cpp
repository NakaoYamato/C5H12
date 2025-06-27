#include "ModelCollision.h"

#include <filesystem>
#include <fstream>
#include <imgui.h>

#include "SerializeFunction.h"
#include "../../Library/DebugSupporter/DebugSupporter.h"

// データのバージョン管理
CEREAL_CLASS_VERSION(ModelCollision, 0)

/// モデル情報読み込み
void ModelCollision::Load(std::weak_ptr<Model> model)
{
	// モデルが設定されていなければエラー
    assert(model.lock() != nullptr);

    _model = model;

    // モデルのシリアライズパスからイベント情報のパスを取得
    if (!Deserialize(_model.lock()->GetFilename()))
    {
        // なかったら新規作成
        Clear();
    }

    _nodeNames.clear();
    // ノードの名前を全取得
    for (auto& node : _model.lock()->GetPoseNodes())
    {
        _nodeNames.push_back(node.name.c_str());
    }
}

/// デバッグ表示
void ModelCollision::DebugRender()
{
    if (!_model.lock())
		return;

	for (auto& sphere : _sphereDatas)
	{
        if (sphere.nodeIndex == -1)
            continue;

		auto& node = _model.lock()->GetPoseNodes()[sphere.nodeIndex];
		Debug::Renderer::DrawSphere(
            sphere.position.TransformCoord(node.worldTransform), 
            sphere.radius, 
            Vector4::White);
	}
	for (auto& capsule : _capsuleDatas)
	{
        if (capsule.startNodeIndex == -1)
            continue;
        if (capsule.endNodeIndex == -1)
            continue;

		auto& startNode = _model.lock()->GetPoseNodes()[capsule.startNodeIndex];
		auto& endNode = _model.lock()->GetPoseNodes()[capsule.endNodeIndex];
		Debug::Renderer::DrawCapsule(
			capsule.start.TransformCoord(startNode.worldTransform),
			capsule.end.TransformCoord(endNode.worldTransform),
			capsule.radius,
			Vector4::White);
	}
}

/// GUI描画
void ModelCollision::DrawGui(bool canEdit)
{
	if (!_model.lock())
		return;
    if (canEdit)
    {
        if (ImGui::Button(u8"球追加"))
        {
            _sphereDatas.push_back(SphereData());
        }
    }
    int index = 0;
	for (auto& sphere : _sphereDatas)
	{
        std::string str = "sphere" + std::to_string(index);
        if (ImGui::TreeNodeEx(str.c_str()))
        {
            // nodeの選択
            ImGui::Combo(u8"ノード", &sphere.nodeIndex, _nodeNames.data(), (int)_nodeNames.size());
            ImGui::DragFloat3(u8"中心座標", &sphere.position.x, 0.01f);
            ImGui::DragFloat(u8"半径", &sphere.radius, 0.01f, 0.0f, 100.0f);

            if (canEdit)
            {
                if (ImGui::Button(u8"削除"))
                {
                    _sphereDatas.erase(_sphereDatas.begin() + index);
                    ImGui::TreePop();
                    break;
                }
            }
            ImGui::TreePop();
        }
        index++;
	}
    ImGui::Separator();
    if (canEdit)
    {
        if (ImGui::Button(u8"カプセル追加"))
        {
            _capsuleDatas.push_back(CapsuleData());
        }
    }
    index = 0;
	for (auto& capsule : _capsuleDatas)
	{
        std::string str = "capsule" + std::to_string(index);
        if (ImGui::TreeNode(str.c_str()))
        {
            // nodeの選択
            ImGui::Combo(u8"開始ノード", &capsule.startNodeIndex, _nodeNames.data(), (int)_nodeNames.size());
            ImGui::Combo(u8"終了ノード", &capsule.endNodeIndex, _nodeNames.data(), (int)_nodeNames.size());
            ImGui::DragFloat3(u8"開始座標", &capsule.start.x, 0.01f);
            ImGui::DragFloat3(u8"終了座標", &capsule.end.x, 0.01f);
            ImGui::DragFloat(u8"半径", &capsule.radius, 0.01f, 0.0f, 100.0f);

            if (canEdit)
            {
                if (ImGui::Button(u8"削除"))
                {
                    _capsuleDatas.erase(_capsuleDatas.begin() + index);
                    ImGui::TreePop();
                    break;
                }
            }
            ImGui::TreePop();
        }
        index++;
	}
}

#pragma region ファイル操作
template<class T>
inline void ModelCollision::SphereData::serialize(T& archive, const std::uint32_t version)
{
    if (version == 0)
    {
        archive(
            CEREAL_NVP(nodeIndex),
            CEREAL_NVP(position),
            CEREAL_NVP(radius)
        );
    }
}

template<class T>
inline void ModelCollision::CapsuleData::serialize(T& archive, const std::uint32_t version)
{
    if (version == 0)
    {
        archive(
            CEREAL_NVP(startNodeIndex),
            CEREAL_NVP(start),
            CEREAL_NVP(endNodeIndex),
            CEREAL_NVP(end),
            CEREAL_NVP(radius)
        );
    }
}

/// データ書き出し
bool ModelCollision::Serialize(const char* filename)
{
    std::filesystem::path serializePath(filename);
    serializePath.replace_extension(MODEL_COLLISION_EXTENSION);

    std::ofstream ostream(serializePath.string().c_str(), std::ios::binary);
    if (ostream.is_open())
    {
        cereal::BinaryOutputArchive archive(ostream);

        try
        {
            archive(
                CEREAL_NVP(_sphereDatas),
                CEREAL_NVP(_capsuleDatas)
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
bool ModelCollision::Deserialize(const char* filename)
{
    std::filesystem::path serializePath(filename);
    serializePath.replace_extension(MODEL_COLLISION_EXTENSION);

    std::ifstream istream(serializePath.string().c_str(), std::ios::binary);
    if (istream.is_open())
    {
        cereal::BinaryInputArchive archive(istream);

        try
        {
            archive(
                CEREAL_NVP(_sphereDatas),
                CEREAL_NVP(_capsuleDatas)
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
