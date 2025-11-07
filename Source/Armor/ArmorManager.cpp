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
			size_t size = jsonData[typeName + "Size"].get<std::size_t>();
			for (size_t i = 0; i < size; ++i)
			{
				ArmorData data;
				data.name			= jsonData[typeName + std::to_string(i) + "name"].get<std::string>();
				data.modelFilePath	= jsonData[typeName + std::to_string(i) + "modelFilePath"].get<std::string>();
				size_t hiddenMeshesSize = jsonData[typeName + std::to_string(i) + "hiddenMeshesSize"].get<std::size_t>();
				for (size_t j = 0; j < hiddenMeshesSize; ++j)
				{
					data.hiddenMeshes.push_back(jsonData[typeName + std::to_string(i) + "hiddenMesh" + std::to_string(j)].get<std::string>());
				}
				data.type		= static_cast<ArmorType>(jsonData[typeName + std::to_string(i) + "type"].get<int>());
				data.defense	= jsonData[typeName + std::to_string(i) + "defense"].get<float>();
				data.rarity		= jsonData[typeName + std::to_string(i) + "rarity"].get<int>();
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
			jsonData[typeName + std::to_string(i) + "name"]				= dataVec[i].name;
			jsonData[typeName + std::to_string(i) + "modelFilePath"]	= dataVec[i].modelFilePath;
			jsonData[typeName + std::to_string(i) + "hiddenMeshesSize"] = dataVec[i].hiddenMeshes.size();
			for (size_t j = 0; j < dataVec[i].hiddenMeshes.size(); ++j)
			{
				jsonData[typeName + std::to_string(i) + "hiddenMesh" + std::to_string(j)] = dataVec[i].hiddenMeshes[j];
			}
			jsonData[typeName + std::to_string(i) + "type"]		= static_cast<int>(dataVec[i].type);
			jsonData[typeName + std::to_string(i) + "defense"]	= dataVec[i].defense;
			jsonData[typeName + std::to_string(i) + "rarity"]	= dataVec[i].rarity;
		}
	}
	return Exporter::SaveJsonFile(_filePath, jsonData);
}

// Gui描画
void ArmorManager::DrawGui()
{
	static const char* ArmorTypeNames[] =
	{
		"Head",
		"Chest",
		"Arm",
		"Waist",
		"Leg"
	};

	ImGui::Separator();
	ImGui::Combo("AddType", &_selectedAddTypeIndex, ArmorTypeNames, _countof(ArmorTypeNames));
	if (ImGui::Button("Add"))
	{
		ArmorData data;
		data.type = static_cast<ArmorType>(_selectedAddTypeIndex);
		_armorDataMap[data.type].push_back(data);
	}
	ImGui::Separator();

	for (auto& [type, dataVec] : _armorDataMap)
	{
		if (ImGui::TreeNode(ToString<ArmorType>(static_cast<size_t>(type)).c_str()))
		{
			int index = 0;
			for (auto& data : dataVec)
			{
				if (ImGui::TreeNode(std::to_string(index).c_str()))
				{
					ImGui::InputText(u8"名前", &data.name);
					ImGui::Text(u8"ファイルパス:");
					ImGui::SameLine();
					ImGui::Text(data.modelFilePath.c_str());
					ImGui::SameLine();
					if (ImGui::Button("..."))
					{
						// ダイアログを開く
						std::string filepath;
						std::string currentDirectory;
						Debug::Dialog::DialogResult result = Debug::Dialog::OpenFileName(filepath, currentDirectory, ImGui::ModelFilter);
						// ファイルを選択したら
						if (result == Debug::Dialog::DialogResult::Yes || result == Debug::Dialog::DialogResult::OK)
						{
							try
							{
								// 相対パス取得
								std::filesystem::path path =
									std::filesystem::relative(filepath, currentDirectory);
								data.modelFilePath = path.u8string();
							}
							catch (...)
							{
								data.modelFilePath = filepath;
							}
						}
					}
					ImGui::InputFloat(u8"防御力", &data.defense);
					ImGui::InputInt(u8"レア度", &data.rarity);
					if (ImGui::TreeNode(u8"非表示メッシュ"))
					{
						size_t hiddenMeshIndex = 0;
						for (auto& meshName : data.hiddenMeshes)
						{
							ImGui::InputText((u8"非表示メッシュ" + std::to_string(hiddenMeshIndex)).c_str(), &meshName);
							ImGui::SameLine();
							if (ImGui::Button((u8"削除##hiddenMesh" + std::to_string(hiddenMeshIndex)).c_str()))
							{
								data.hiddenMeshes.erase(data.hiddenMeshes.begin() + hiddenMeshIndex);
								break;
							}
							hiddenMeshIndex++;
						}
						ImGui::TreePop();
					}

					if (ImGui::Button(u8"削除"))
					{
						dataVec.erase(dataVec.begin() + index);
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
