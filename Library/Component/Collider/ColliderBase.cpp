#include "ColliderBase.h"

#include "../../External/nameof/include/nameof.hpp"
#include "../../External/magic_enum/include/magic_enum/magic_enum.hpp"
#include <imgui.h>
// GUI描画
void ColliderBase::DrawGui()
{
	if (ImGui::TreeNode(u8"レイヤー"))
	{
		for (auto layer : magic_enum::enum_values<CollisionLayer>())
		{
			bool isChecked = (static_cast<unsigned int>(_layer) & static_cast<unsigned int>(layer));
			if (ImGui::Checkbox(magic_enum::enum_name(layer).data(), &isChecked))
			{
				unsigned int layerValue = static_cast<unsigned int>(layer);
				if (isChecked)
				{
                    _layer = static_cast<CollisionLayer>(static_cast<unsigned int>(_layer) | layerValue);
				}
				else
				{
                    _layer = static_cast<CollisionLayer>(static_cast<unsigned int>(_layer) & ~layerValue);
				}
			}
		}
        ImGui::TreePop();
	}
	if (ImGui::TreeNode(u8"レイヤーマスク"))
	{
		for (auto layer : magic_enum::enum_values<CollisionLayer>())
		{
			bool isChecked = (_layerMask & static_cast<unsigned int>(layer)) != 0;
			if (ImGui::Checkbox(magic_enum::enum_name(layer).data(), &isChecked))
			{
				unsigned int layerValue = static_cast<unsigned int>(layer);
				if (isChecked)
				{
					_layerMask |= layerValue;
				}
				else
				{
					_layerMask &= ~layerValue;
				}
			}
		}
		ImGui::TreePop();
    }
	ImGui::Checkbox(u8"有効", &_isActive);
	ImGui::Checkbox(u8"トリガー", &_isTrigger);
}

/// 接触の解消処理
void ColliderBase::OnContact(CollisionData& collisionData)
{   
}

bool ColliderBase::LoadFromFile()
{
	// 現在のシーン名、アクター名からファイルパスを生成
	std::string filePath = this->GetDirectory();
	filePath += this->GetName();
	filePath += ".json";

	nlohmann::json jsonData;
	if (!Exporter::LoadJsonFile(filePath.c_str(), &jsonData))
		return false;
	// 各データ読み込み
	SetLayer(jsonData.value("layer", CollisionLayer::None));
	SetLayerMask(jsonData.value("layerMask", CollisionLayerMaskAll));
	SetActive(jsonData.value("isActive", true));
	SetTrigger(jsonData.value("isTrigger", false));
	return true;
}

bool ColliderBase::SaveToFile()
{
	// 現在のシーン名、アクター名からファイルパスを生成
	std::string filePath = this->GetDirectory();

	// ディレクトリ確保
	std::filesystem::path outputDirPath(filePath);
	if (!std::filesystem::exists(outputDirPath))
	{
		// なかったらディレクトリ作成
		std::filesystem::create_directories(outputDirPath);
	}

	filePath += this->GetName();
	filePath += ".json";

	nlohmann::json jsonData;
	jsonData["layer"] = GetLayer();
	jsonData["layerMask"] = GetLayerMask();
	jsonData["isActive"] = IsActive();
	jsonData["isTrigger"] = IsTrigger();

	return Exporter::SaveJsonFile(filePath.c_str(), jsonData);
}
