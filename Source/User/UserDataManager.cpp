#include "UserDataManager.h"

#include "../../Library/Algorithm/Converter.h"

#include <Mygui.h>

// マネージャーから元データ取得
inline ArmorData* UserDataManager::ArmorUserData::GetBaseData() const
{
	auto armorManager = ResourceManager::Instance().GetResourceAs<ArmorManager>("ArmorManager");
	if (!armorManager)
		return nullptr;
	return armorManager->GetArmorData(type, index);
}

// Gui描画
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

// マネージャーから元データ取得
inline ItemData* UserDataManager::ItemUserData::GetBaseData() const
{
	auto itemManager = ResourceManager::Instance().GetResourceAs<ItemManager>("ItemManager");
	if (!itemManager)
		return nullptr;
	return itemManager->GetItemData(index);
}

// Gui描画
inline void UserDataManager::ItemUserData::DrawGui()
{
	auto itemManager = ResourceManager::Instance().GetResourceAs<ItemManager>("ItemManager");
	if (!itemManager)
		return;

	ItemData* baseData = GetBaseData();
	if (baseData)
	{
		// アイテムアイコンGui描画
		itemManager->DrawItemIconGui(baseData->iconIndex, baseData->overlayIconIndex, baseData->color);
		ImGui::SameLine();
		ImGui::Text(baseData->name.c_str());
	}

	ImGui::InputInt(u8"所持数", &quantity);
	ImGui::DragFloat(u8"入手時間", &acquisitionTime, 0.1f);
	if (baseData)
	{
		if (ImGui::TreeNode((std::string("Base") + std::to_string(index)).c_str()))
		{
			baseData->DrawGui(itemManager->GetItemIconTextureIndex());
			ImGui::TreePop();
		}
	}
}

// 初期化処理
bool UserDataManager::Initialize()
{
	auto itemManager = ResourceManager::Instance().GetResourceAs<ItemManager>("ItemManager");
	if (!itemManager)
		return false;

	// アイテムの情報読み込み
	for (int i = 0; i < static_cast<int>(itemManager->GetItemDataList().size()); ++i)
	{
		// 要素が存在しなければ追加
		if (_acquiredItemMap.find(i) == _acquiredItemMap.end())
		{
			ItemUserData data;
			data.index = i;
			data.quantity = 0;
			_acquiredItemMap[i] = data;
		}
	}

	_emptyPouchItemData.pouchIndex = -1;
	_emptyPouchItemData.itemIndex = -1;
	_emptyPouchItemData.quantity = 0;

	return true;
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

		if (!jsonData.contains("AcquiredItemListSize"))
			return false;
		int itemListSize = static_cast<int>(jsonData["AcquiredItemListSize"].get<std::size_t>());
		for (int i = 0; i < itemListSize; ++i)
		{
			auto& sub = jsonData["AcquiredItemList" + std::to_string(i)];
			ItemUserData data;
			data.index				= sub.value("index", data.index);
			data.quantity			= sub.value("quantity", data.quantity);
			data.acquisitionTime	= sub.value("acquisitionTime", data.acquisitionTime);
			_acquiredItemMap[i] = data;
		}
		for (size_t i = 0; i <= static_cast<size_t>(ArmorType::Leg); ++i)
		{
			std::string key = "EquippedArmorIndex" + std::to_string(i);
			if (jsonData.contains(key))
			{
				_equippedArmorIndices[i] = jsonData[key].get<int>();
			}
		}
		for (int i = 0; i < MaxPouchItemCount; ++i)
		{
			auto& sub = jsonData["PouchItem" + std::to_string(i)];
			_pouchItems[i].pouchIndex = i;
			if (!sub.is_object())
				continue;
			_pouchItems[i].itemIndex = sub.value("itemIndex", -1);
			_pouchItems[i].quantity = sub.value("quantity", 0);
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
	jsonData["AcquiredItemListSize"] = _acquiredItemMap.size();
	for (int i = 0; i < static_cast<int>(_acquiredItemMap.size()); ++i)
	{
		auto& sub = jsonData["AcquiredItemList" + std::to_string(i)];
		sub["index"]			= _acquiredItemMap[i].index;
		sub["quantity"]			= _acquiredItemMap[i].quantity;
		sub["acquisitionTime"]	= _acquiredItemMap[i].acquisitionTime;
	}

	for (size_t i = 0; i <= static_cast<size_t>(ArmorType::Leg); ++i)
	{
		jsonData["EquippedArmorIndex" + std::to_string(i)] = _equippedArmorIndices[i];
	}

	for (size_t i = 0; i < MaxPouchItemCount; ++i)
	{
		auto& sub = jsonData["PouchItem" + std::to_string(i)];
		sub["itemIndex"]	= _pouchItems[i].itemIndex;
		sub["quantity"]		= _pouchItems[i].quantity;
	}

	return Exporter::SaveJsonFile(_filePath, jsonData);
}

// Gui描画
void UserDataManager::DrawGui()
{
	if (ImGui::TreeNode(u8"防具GUI"))
	{
		// 防具Gui描画
		DrawAromrGui();
		ImGui::TreePop();
	}
	if (ImGui::TreeNode(u8"アイテムGUI"))
	{
		// アイテムGui描画
		DrawItemGui();
		ImGui::TreePop();
	}
}

#pragma region 防具
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
#pragma endregion

#pragma region アイテム
// アイテムを使用する
ItemFunctionBase* UserDataManager::UseItem(int pouchIndex, Actor* user)
{
	auto pouch = GetPouchItem(pouchIndex);
	if (pouch->itemIndex < 0)
		return nullptr;
	auto itemData = GetAcquiredItemData(pouch->itemIndex);
	if (!itemData || itemData->quantity <= 0)
		return nullptr;

	auto itemManager = ResourceManager::Instance().GetResourceAs<ItemManager>("ItemManager");
	if (!itemManager)
		return nullptr;

	// 所持数を減らす
	if (pouch->quantity > 0)
	{
		pouch->quantity--;
		// 所持数が0になった場合はポーチから削除
		if (pouch->quantity == 0)
		{
			pouch->itemIndex = -1;
			SortPouchItems();
		}
	}

	// 関数取得
	auto itemFunction = itemManager->GetItemFunction(pouch->itemIndex);
	if (itemFunction)
	{
		// 使用処理
		itemFunction->Start(itemData->GetBaseData(), user);
	}

	return itemFunction;
}
// ポーチの整理
void UserDataManager::SortPouchItems()
{
	// 空データを詰める
	for (int i = 0; i < MaxPouchItemCount - 1; ++i)
	{
		if (_pouchItems[i].itemIndex != -1)
			continue;
		for (int j = i + 1; j < MaxPouchItemCount; ++j)
		{
			if (_pouchItems[j].itemIndex == -1)
				continue;
			_pouchItems[i].itemIndex = _pouchItems[j].itemIndex;
			_pouchItems[i].quantity = _pouchItems[j].quantity;
			_pouchItems[j].itemIndex = -1;
			_pouchItems[j].quantity = 0;
			break;
		}
	}
}

// アイテムデータ取得
UserDataManager::ItemUserData* UserDataManager::GetAcquiredItemData(int index)
{
	return &_acquiredItemMap[index];
}

// アイテムポーチ内のアイテムインデックス取得
UserDataManager::PouchItemData* UserDataManager::GetPouchItem(int pouchIndex)
{
	auto lastPouchItem = GetLastPouchItem();
	int maxCount = lastPouchItem->pouchIndex + 2;
	int index = pouchIndex % maxCount;
	if (index < 0)
		index += maxCount;

	if (index == lastPouchItem->pouchIndex + 1)
		return &_emptyPouchItemData;

	return &_pouchItems[index];
}

// アイテムポーチ内の最後のアイテムポーチ取得
UserDataManager::PouchItemData* UserDataManager::GetLastPouchItem()
{
	// 先頭のポーチが空の場合は先頭を返す
	if (_pouchItems[0].itemIndex == -1)
		return &_pouchItems[0];

	for (int i = 1; i < MaxPouchItemCount; i++)
	{
		if (_pouchItems[i].itemIndex == -1)
			return &_pouchItems[i - 1];
	}

	// 全てデータがあるの場合は最後のポーチを返す
	return &_pouchItems[MaxPouchItemCount - 1];
}

// アイテムポーチ内のアイテムインデックス変更
void UserDataManager::SetPouchItemIndex(int pouchIndex, int itemIndex)
{
	// 範囲チェック
	if (pouchIndex < 0 || pouchIndex >= MaxPouchItemCount)
		return;
	// 変更がない場合は処理しない
	if (_pouchItems[pouchIndex].itemIndex == itemIndex)
		return;

	// 変更先がポーチに含めないアイテムの場合は次のアイテムに変更
	if (_acquiredItemMap[itemIndex].GetBaseData() &&
		!_acquiredItemMap[itemIndex].GetBaseData()->isInPouch)
		return SetPouchItemIndex(pouchIndex, 
			_pouchItems[pouchIndex].itemIndex < itemIndex ?
			itemIndex + 1 :
			itemIndex - 1);

	// 空にする場合
	if (itemIndex >= _acquiredItemMap.size() || itemIndex == -1)
	{
		_pouchItems[pouchIndex].itemIndex = -1;
		return;
	}

	// そのアイテムがセットできるか判定
	for (int i = 0; i < MaxPouchItemCount; ++i)
	{
		if (i == pouchIndex)
			continue;

		// 他のポーチスロットと重複している場合は次のアイテムに変更
		if (_pouchItems[i].itemIndex == itemIndex)
		{
			// 最初からチェックし直す
			return SetPouchItemIndex(pouchIndex,
				_pouchItems[pouchIndex].itemIndex < itemIndex ?
				itemIndex + 1 :
				itemIndex - 1);
		}
	}
	// 問題なければ変更
	_pouchItems[pouchIndex].itemIndex = itemIndex;
	// 所持数初期化
	_pouchItems[pouchIndex].quantity = 0;
}
#pragma endregion

// 防具Gui描画
void UserDataManager::DrawAromrGui()
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

// アイテムGui描画
void UserDataManager::DrawItemGui()
{
	auto itemManager = ResourceManager::Instance().GetResourceAs<ItemManager>("ItemManager");
	if (!itemManager)
		return;

	if (ImGui::TreeNode(u8"アイテム所持状況"))
	{
		for (auto& [index, userData] : _acquiredItemMap)
		{
			std::string label = "Item" + std::to_string(index);
			if (auto baseData = userData.GetBaseData())
			{
				label += ":" + baseData->name;
			}

			if (ImGui::TreeNode(label.c_str()))
			{
				userData.DrawGui();
				ImGui::TreePop();
			}
		}
		ImGui::TreePop();
	}

	if (ImGui::TreeNode(u8"アイテムポーチ内のアイテムインデックス"))
	{
		static float ColumnWidths[4] = { 0.0f, 250.0f, 180.0f, 200.0f };
		static float InputWidth = 100.0f;

		ImGui::Columns(4, "my_columns_id", true);
		ImGui::SetColumnWidth(0, ColumnWidths[0]);
		ImGui::SetColumnWidth(1, ColumnWidths[1]);
		ImGui::SetColumnWidth(2, ColumnWidths[2]);
		ImGui::SetColumnWidth(3, ColumnWidths[3]);

		for (int i = 0; i < MaxPouchItemCount; ++i)
		{
			ImGui::PushID(static_cast<int>(i));

			// ポーチスロット番号
			ImGui::NextColumn();
			ImGui::SetNextItemWidth(InputWidth);
			int itemIndex = _pouchItems[i].itemIndex;
			if (ImGui::InputInt((u8"ポーチスロット" + std::to_string(i)).c_str(), &itemIndex))
				SetPouchItemIndex(i, itemIndex);

			auto baseData = _pouchItems[i].itemIndex != -1 ?
				_acquiredItemMap[_pouchItems[i].itemIndex].GetBaseData() : nullptr;

			// アイテム名表示
			ImGui::NextColumn();
			ImGui::Text(u8"%s", baseData ? baseData->name.c_str() : "None");

			// 所持数
			ImGui::NextColumn();
			if (baseData)
			{
				if (baseData->maxCountInpouch == -1)
				{
					ImGui::Text(u8"所持数:無限");
					_pouchItems[i].quantity = -1;
				}
				else
				{
					ImGui::SetNextItemWidth(InputWidth);
					ImGui::InputInt(u8"所持数", &_pouchItems[i].quantity);
					_pouchItems[i].quantity = std::clamp(
						_pouchItems[i].quantity, 0,
						baseData->maxCountInpouch);
				}
			}

			ImGui::NextColumn();
			ImGui::PopID();

			// このポーチにアイテムが設定されていないなら終了
			if (_pouchItems[i].itemIndex == -1)
				break;
		}

		ImGui::Columns(1);

		ImGui::TreePop();
	}
}
