#include "ArmorManager.h"

#include "../../Library/Exporter/Exporter.h"
#include "../../Library/Algorithm/Converter.h"

#include <Mygui.h>

// ファイル読み込み
bool ArmorManager::LoadFromFile()
{
	nlohmann::json jsonData;
	if (Exporter::LoadJsonFile(_filePath, &jsonData))
	{
		_armorDataMap.clear();
		_armorDataMap[ArmorType::Head] = std::vector<ArmorData>();
		_armorDataMap[ArmorType::Chest] = std::vector<ArmorData>();
		_armorDataMap[ArmorType::Arm] = std::vector<ArmorData>();
		_armorDataMap[ArmorType::Waist] = std::vector<ArmorData>();
		_armorDataMap[ArmorType::Leg] = std::vector<ArmorData>();

		for (auto& [type, dataVec] : _armorDataMap)
		{
			std::string typeName = ToString<ArmorType>(static_cast<size_t>(type));
			if (!jsonData.contains(typeName + "Size"))
				continue;
			size_t size = jsonData[typeName + "Size"].get<std::size_t>();
			for (size_t i = 0; i < size; ++i)
			{
				auto& sub = jsonData[typeName + std::to_string(i)];
				ArmorData data;
				data.Load(sub);
				dataVec.push_back(data);
			}
		}

		return true;
	}
	return false;
}

// ファイル保存
bool ArmorManager::SaveToFile()
{
	nlohmann::json jsonData;
	for (auto& [type, dataVec] : _armorDataMap)
	{
		std::string typeName = ToString<ArmorType>(static_cast<size_t>(type));

		jsonData[typeName + "Size"] = dataVec.size();
		for (size_t i = 0; i < dataVec.size(); ++i)
		{
			auto& sub = jsonData[typeName + std::to_string(i)];
			dataVec[i].Save(sub);
		}
	}
	return Exporter::SaveJsonFile(_filePath, jsonData);
}

// Gui描画
void ArmorManager::DrawGui()
{
	for (size_t i = 0; i <= static_cast<size_t>(ArmorType::Leg); ++i)
	{
		if (ImGui::TreeNode(ToString<ArmorType>(i).c_str()))
		{
			if (ImGui::Button("Add"))
			{
				ArmorData data;
				data.type = static_cast<ArmorType>(i);
				_armorDataMap[data.type].push_back(data);
			}
			ImGui::Separator();

			int index = 0;
			for (auto& data : _armorDataMap[static_cast<ArmorType>(i)])
			{
				if (ImGui::TreeNode(std::to_string(index).c_str()))
				{
					data.DrawGui();

					if (ImGui::Button(u8"削除"))
					{
						_armorDataMap[static_cast<ArmorType>(i)].erase(_armorDataMap[static_cast<ArmorType>(i)].begin() + index);
						ImGui::TreePop();
						break;
					}

					ImGui::TreePop();
				}
				index++;
			}

			ImGui::TreePop();
		}
	}
}

// 防具データ取得
ArmorData* ArmorManager::GetArmorData(ArmorType type, size_t index)
{
	if (_armorDataMap.find(type) == _armorDataMap.end())
		return nullptr;
	if (index >= _armorDataMap[type].size())
		return nullptr;

	return &_armorDataMap[type][index];
}
