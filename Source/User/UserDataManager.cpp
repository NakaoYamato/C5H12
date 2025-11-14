#include "UserDataManager.h"

#include "../../Library/Algorithm/Converter.h"

#include <Mygui.h>

inline ArmorData* UserDataManager::ArmorUserData::GetBaseData() const
{
	auto armorManager = ResourceManager::Instance().GetResourceAs<ArmorManager>("ArmorManager");
	if (!armorManager)
		return nullptr;
	return armorManager->GetArmorData(type, index);
}

inline void UserDataManager::ArmorUserData::DrawGui()
{
	auto armorManager = ResourceManager::Instance().GetResourceAs<ArmorManager>("ArmorManager");
	if (!armorManager)
		return;

	int maxCount = static_cast<int>(armorManager->GetArmorDataList(type).size()) - 1;

	ImGui::InputInt(u8"参照番号", &index);
	index = std::clamp(index, 0, maxCount);
	ImGui::DragFloat(u8"入手時間", &acquisitionTime, 0.1f);
	ImGui::InputInt(u8"レベル", &level);
	ImGui::InputInt(u8"経験値", &experience);
	ImGui::ColorEdit4(u8"Color", &color.x);

	ArmorData* baseData = GetBaseData();
	if (baseData)
	{
		if (ImGui::TreeNode((std::string("Base") + std::to_string(index)).c_str()))
		{
			baseData->DrawGui();
			ImGui::TreePop();
		}
	}
}

inline ItemData* UserDataManager::ItemUserData::GetBaseData() const
{
	auto itemManager = ResourceManager::Instance().GetResourceAs<ItemManager>("ItemManager");
	if (!itemManager)
		return nullptr;
	return itemManager->GetItemData(index);
}

inline void UserDataManager::ItemUserData::DrawGui()
{
}

// ファイル読み込み
bool UserDataManager::LoadFromFile()
{
	nlohmann::json jsonData;
	if (Exporter::LoadJsonFile(_filePath, &jsonData))
	{
		_acquiredArmorMap.clear();
		_acquiredArmorMap[ArmorType::Head]	= std::vector<ArmorUserData>();
		_acquiredArmorMap[ArmorType::Chest] = std::vector<ArmorUserData>();
		_acquiredArmorMap[ArmorType::Arm]	= std::vector<ArmorUserData>();
		_acquiredArmorMap[ArmorType::Waist] = std::vector<ArmorUserData>();
		_acquiredArmorMap[ArmorType::Leg]	= std::vector<ArmorUserData>();

		for (auto& [type, dataVec] : _acquiredArmorMap)
		{
			std::string typeName = ToString<ArmorType>(static_cast<size_t>(type));
			if (!jsonData.contains(typeName + "Size"))
				continue;
			size_t size = jsonData[typeName + "Size"].get<std::size_t>();
			for (size_t i = 0; i < size; ++i)
			{
				auto& sub = jsonData[typeName + std::to_string(i)];
				ArmorUserData data;
				data.type				= static_cast<ArmorType>(sub.value("type", static_cast<int>(data.type)));
				data.index				= sub.value("index", data.index);
				data.acquisitionTime	= sub.value("acquisitionTime", data.acquisitionTime);
				data.level				= sub.value("level", data.level);
				data.experience			= sub.value("experience", data.experience);
				data.color.x			= sub.value("colorX", data.color.x);
				data.color.y			= sub.value("colorY", data.color.y);
				data.color.z			= sub.value("colorZ", data.color.z);
				data.color.w			= sub.value("colorW", data.color.w);
				dataVec.push_back(data);
			}
		}
		_acquiredItemList.clear();
		if (!jsonData.contains("AcquiredItemListSize"))
			return false;
		size_t itemListSize = jsonData["AcquiredItemListSize"].get<std::size_t>();
		for (size_t i = 0; i < itemListSize; ++i)
		{
			auto& sub = jsonData["AcquiredItemList" + std::to_string(i)];
			ItemUserData data;
			data.index				= sub.value("index", data.index);
			data.quantity			= sub.value("quantity", data.quantity);
			data.acquisitionTime	= sub.value("acquisitionTime", data.acquisitionTime);
			_acquiredItemList.push_back(data);
		}
		for (size_t i = 0; i <= static_cast<size_t>(ArmorType::Leg); ++i)
		{
			std::string key = "EquippedArmorIndex" + std::to_string(i);
			if (jsonData.contains(key))
			{
				_equippedArmorIndices[i] = jsonData[key].get<int>();
			}
		}
		for (size_t i = 0; i < MaxPouchItemCount; ++i)
		{
			std::string key = "ItemPouchIndex" + std::to_string(i);
			if (jsonData.contains(key))
			{
				_itemPouchIndices[i] = jsonData[key].get<int>();
			}
		}
		return true;
	}
	return false;
}

// ファイル保存
bool UserDataManager::SaveToFile()
{
	nlohmann::json jsonData;
	for (auto& [type, dataVec] : _acquiredArmorMap)
	{
		std::string typeName = ToString<ArmorType>(static_cast<size_t>(type));
		jsonData[typeName + "Size"] = dataVec.size();
		for (size_t i = 0; i < dataVec.size(); ++i)
		{
			auto& sub = jsonData[typeName + std::to_string(i)];
			sub["type"]				= static_cast<int>(dataVec[i].type);
			sub["index"]			= dataVec[i].index;
			sub["acquisitionTime"]	= dataVec[i].acquisitionTime;
			sub["level"]			= dataVec[i].level;
			sub["experience"]		= dataVec[i].experience;
			sub["colorX"]			= dataVec[i].color.x;
			sub["colorY"]			= dataVec[i].color.y;
			sub["colorZ"]			= dataVec[i].color.z;
			sub["colorW"]			= dataVec[i].color.w;
		}
	}
	jsonData["AcquiredItemListSize"] = _acquiredItemList.size();
	for (size_t i = 0; i < _acquiredItemList.size(); ++i)
	{
		auto& sub = jsonData["AcquiredItemList" + std::to_string(i)];
		sub["index"]			= _acquiredItemList[i].index;
		sub["quantity"]			= _acquiredItemList[i].quantity;
		sub["acquisitionTime"]	= _acquiredItemList[i].acquisitionTime;
	}

	for (size_t i = 0; i <= static_cast<size_t>(ArmorType::Leg); ++i)
	{
		jsonData["EquippedArmorIndex" + std::to_string(i)] = _equippedArmorIndices[i];
	}

	for (size_t i = 0; i < MaxPouchItemCount; ++i)
	{
		jsonData["ItemPouchIndex" + std::to_string(i)] = _itemPouchIndices[i];
	}

	return Exporter::SaveJsonFile(_filePath, jsonData);
}

// Gui描画
void UserDataManager::DrawGui()
{
	auto armorManager = ResourceManager::Instance().GetResourceAs<ArmorManager>("ArmorManager");
	if (!armorManager)
		return;

	if (ImGui::TreeNode(u8"所持している装備一覧"))
	{
		for (size_t i = 0; i <= static_cast<size_t>(ArmorType::Leg); ++i)
		{
			ArmorType type = static_cast<ArmorType>(i);
			if (ImGui::TreeNode(ToString<ArmorType>(i).c_str()))
			{
				if (ImGui::Button("Add"))
				{
					ArmorUserData data;
					data.type = static_cast<ArmorType>(i);
					_acquiredArmorMap[type].push_back(data);
				}
				ImGui::Separator();

				auto& armorList = _acquiredArmorMap[type];
				int index = 0;
				for (auto& userData : armorList)
				{
					std::string label = ToString<ArmorType>(i) + std::to_string(index);
					if (ImGui::TreeNode(label.c_str()))
					{
						userData.DrawGui();

						if (ImGui::Button(u8"削除"))
						{
							armorList.erase(armorList.begin() + index);
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

		ImGui::TreePop();
	}

	if (ImGui::TreeNode(u8"装備中の防具インデックス"))
	{
		for (size_t i = 0; i <= static_cast<size_t>(ArmorType::Leg); ++i)
		{
			ArmorType type = static_cast<ArmorType>(i);
			ImGui::InputInt(ToString<ArmorType>(i).c_str(), &_equippedArmorIndices[i]);
			_equippedArmorIndices[i] = std::clamp(
				_equippedArmorIndices[i],
				-1,
				static_cast<int>(_acquiredArmorMap[type].size()) - 1);
		}
		ImGui::TreePop();
	}
}

// 防具データ取得
UserDataManager::ArmorUserData* UserDataManager::GetAcquiredArmorData(ArmorType type, int index)
{
	auto armorManager = ResourceManager::Instance().GetResourceAs<ArmorManager>("ArmorManager");
	if (!armorManager)
		return nullptr;

	// 範囲チェック
	if (index < 0 || index >= _acquiredArmorMap[type].size())
		return nullptr;

	return &_acquiredArmorMap[type][index];
}

// 装備中の防具データ取得
UserDataManager::ArmorUserData* UserDataManager::GetEquippedArmorData(ArmorType type)
{
	return GetAcquiredArmorData(
		type,
		_equippedArmorIndices[static_cast<int>(type)]);
}

// 所持している防具データリスト取得
std::vector<UserDataManager::ArmorUserData>& UserDataManager::GetAcquiredArmorDataList(ArmorType type)
{
	return _acquiredArmorMap[type];
}

// 装備中の防具インデックス取得
int UserDataManager::GetEquippedArmorIndex(ArmorType type) const
{
	return _equippedArmorIndices[static_cast<int>(type)];
}

// 装備中の防具インデックス変更
void UserDataManager::SetEquippedArmorIndex(ArmorType type, int index)
{
	_equippedArmorIndices[static_cast<int>(type)] = index;
}
