#include "PlayerArmorController.h"

#include "../../Library/Exporter/Exporter.h"
#include "../../Library/Algorithm/Converter.h"

#include <Mygui.h>

// 生成時処理
void PlayerArmorController::OnCreate()
{
	LoadArmorData();
}

// 更新処理
void PlayerArmorController::Update(float elapsedTime)
{
}

// GUI描画
void PlayerArmorController::DrawGui()
{
	if (ImGui::Button(u8"保存"))
	{
		SaveArmorData();
	}
	if (ImGui::InputInt(u8"頭防具インデックス", &_armorIndices[static_cast<int>(ArmorType::Head)]))
	{
		SetArmorIndex(ArmorType::Head, _armorIndices[static_cast<int>(ArmorType::Head)]);
	}
	if (ImGui::InputInt(u8"胴防具インデックス", &_armorIndices[static_cast<int>(ArmorType::Chest)]))
	{
		SetArmorIndex(ArmorType::Chest, _armorIndices[static_cast<int>(ArmorType::Chest)]);
	}
	if (ImGui::InputInt(u8"腕防具インデックス", &_armorIndices[static_cast<int>(ArmorType::Arm)]))
	{
		SetArmorIndex(ArmorType::Arm, _armorIndices[static_cast<int>(ArmorType::Arm)]);
	}
	if (ImGui::InputInt(u8"腰防具インデックス", &_armorIndices[static_cast<int>(ArmorType::Waist)]))
	{
		SetArmorIndex(ArmorType::Waist, _armorIndices[static_cast<int>(ArmorType::Waist)]);
	}
	if (ImGui::InputInt(u8"脚防具インデックス", &_armorIndices[static_cast<int>(ArmorType::Leg)]))
	{
		SetArmorIndex(ArmorType::Leg, _armorIndices[static_cast<int>(ArmorType::Leg)]);
	}
}

// 防具インデックス設定
void PlayerArmorController::SetArmorIndex(ArmorType type, int index)
{
	auto armorManager = ResourceManager::Instance().GetResourceAs<ArmorManager>("ArmorManager");
	if (!armorManager)
		return;

	// データがあるか確認
	ArmorData* armorData = armorManager->GetArmorData(ArmorType::Chest, index);
	if (!armorData)
	{
		_armorIndices[static_cast<int>(type)] = -1;
	}
	else
	{
		_armorIndices[static_cast<int>(type)] = index;
	}

	// アクターが存在する場合は更新
	auto armorActor = GetArmorActor(type);
	if (armorActor)
	{
		armorActor->BuildData(armorManager.get(), _armorIndices[static_cast<int>(type)]);
	}
}

void PlayerArmorController::LoadArmorData()
{
	nlohmann::json jsonData;
	if (Exporter::LoadJsonFile(_filePath, &jsonData))
	{
		for (size_t i = 0; i <= static_cast<size_t>(ArmorType::Leg); ++i)
		{
			std::string typeName = ToString<ArmorType>(i);
			if (jsonData.contains(typeName + "index"))
			{
				_armorIndices[i] = jsonData[typeName + "index"].get<int>();
			}
		}
	}
}

void PlayerArmorController::SaveArmorData()
{
	nlohmann::json jsonData;
	for (size_t i = 0; i <= static_cast<size_t>(ArmorType::Leg); ++i)
	{
		std::string typeName = ToString<ArmorType>(i);
		jsonData[typeName + "index"] = _armorIndices[i];
	}
	Exporter::SaveJsonFile(_filePath, jsonData);
}
