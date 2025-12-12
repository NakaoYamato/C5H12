#include "WeaponManager.h"

#include "../../Library/Exporter/Exporter.h"
#include "../../Library/Algorithm/Converter.h"

#include <Mygui.h>

// 初期化処理
bool WeaponManager::Initialize()
{
	for (size_t i = 0; i < static_cast<size_t>(WeaponType::WeaponTypeMax); ++i)
	{
		WeaponType type = static_cast<WeaponType>(i);
		if (_weaponDataMap.find(type) == _weaponDataMap.end())
			_weaponDataMap[type] = std::vector<WeaponData>();
	}
	return true;
}

// ファイル読み込み
bool WeaponManager::LoadFromFile()
{
	nlohmann::json jsonData;
	if (Exporter::LoadJsonFile(_filePath, &jsonData))
	{
		_weaponDataMap.clear();
		for (size_t i = 0; i < static_cast<size_t>(WeaponType::WeaponTypeMax); ++i)
		{
			WeaponType type = static_cast<WeaponType>(i);
			_weaponDataMap[type] = std::vector<WeaponData>();
			std::string typeName = ToString<WeaponType>(static_cast<size_t>(type));
			if (!jsonData.contains(typeName + "Size"))
				continue;
			size_t size = jsonData[typeName + "Size"].get<std::size_t>();
			for (size_t i = 0; i < size; ++i)
			{
				auto& sub = jsonData[typeName + std::to_string(i)];
				WeaponData data;
				data.Load(sub);
				_weaponDataMap[type].push_back(data);
			}
		}

		return true;
	}
	return false;
}

// ファイル保存
bool WeaponManager::SaveToFile()
{
	nlohmann::json jsonData;
	for (auto& [type, dataVec] : _weaponDataMap)
	{
		std::string typeName = ToString<WeaponType>(static_cast<size_t>(type));
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
void WeaponManager::DrawGui()
{
	for (auto& [type, dataVec] : _weaponDataMap)
	{
		std::string typeName = ToString<WeaponType>(static_cast<size_t>(type));
		if (ImGui::TreeNode(typeName.c_str()))
		{
			if (ImGui::Button((u8"追加##" + typeName).c_str()))
			{
				dataVec.push_back(WeaponData());
			}

			for (size_t i = 0; i < dataVec.size(); ++i)
			{
				if (ImGui::TreeNode(std::to_string(i).c_str()))
				{
					dataVec[i].DrawGui();
					ImGui::TreePop();
				}
			}
			ImGui::TreePop();
		}
	}
}

// 武器データ取得
WeaponData* WeaponManager::GetWeaponData(WeaponType type, size_t index)
{
	auto& dataVec = _weaponDataMap[type];
	if (index < dataVec.size())
	{
		return &dataVec[index];
	}
	return nullptr;
}
