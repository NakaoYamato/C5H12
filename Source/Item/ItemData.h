#pragma once

#include "../../Library/Math/Vector.h"
#include "../../Library/Exporter/Exporter.h"
#include "../../Library/Algorithm/Converter.h"

#include <string>
#include <vector>
#include <Mygui.h>

enum class ItemType
{
	Drinkable,
	Useable,
	Installable,

	Material,

	ItemTypeMax,
};

struct ItemData
{
	std::string		name			= "";					// アイテムの名前
	int 			iconIndex		= -1;					// アイテムアイコンの番号
	Vector4			color			= Vector4::White;		// アイテムの色
	ItemType		type			= ItemType::Drinkable;	// アイテムの種類
	bool			isInPouch		= true;					// ポーチ内に入るかどうか
	int				maxCountInpouch = 99;					// ポーチ内最大所持数 -1は無限
	int				rarity			= 0;					// レア度
	int				overlayIconIndex = -1;					// アイテムオーバーレイアイコンの番号
	int 			executeProcessIndex = -1;				// アイテム実行処理のインデックス

	inline void DrawGui(int itemIconTextureIndex)
	{
		static std::vector<char> ItemTypeNames;
		if (ItemTypeNames.empty())
		{
			for (size_t i = 0; i < static_cast<size_t>(ItemType::ItemTypeMax); ++i)
			{
				for (char c : ToString<ItemType>(i))
				{
					ItemTypeNames.push_back(c);
				}
				ItemTypeNames.push_back('\0');
			}
		}

		ImGui::InputText(u8"名前", &name);
		if (ImGui::InputInt(u8"アイコン番号", &iconIndex))
			iconIndex = std::clamp<int>(iconIndex, -1, itemIconTextureIndex - 1);
		if (ImGui::InputInt(u8"オーバーレイアイコン番号", &overlayIconIndex))
			overlayIconIndex = std::clamp<int>(overlayIconIndex, -1, 7);
		ImGui::ColorEdit4(u8"表示色", &color.x);
		ImGui::Combo(u8"種類", reinterpret_cast<int*>(&type), ItemTypeNames.data(), static_cast<int>(ItemType::ItemTypeMax));
		ImGui::Checkbox(u8"ポーチ内に入るかどうか", &isInPouch);
		ImGui::InputInt(u8"ポーチ内最大所持数", &maxCountInpouch);
		maxCountInpouch = std::clamp<int>(maxCountInpouch, -1, 99);
		ImGui::InputInt(u8"レア度", &rarity);
	}

	// データ保存
	inline void Load(nlohmann::json_abi_v3_12_0::json& json)
	{
		name				= json.value("name", name);
		iconIndex			= json.value("iconIndex", iconIndex);
		color.x				= json.value("colorX", color.x);
		color.y				= json.value("colorY", color.y);
		color.z				= json.value("colorZ", color.z);
		color.w				= json.value("colorW", color.w);
		type				= static_cast<ItemType>(json.value("type", static_cast<int>(type)));
		isInPouch			= json.value("isInPouch", isInPouch);
		maxCountInpouch		= json.value("maxCountInpouch", maxCountInpouch);
		rarity				= json.value("rarity", rarity);
		overlayIconIndex	= json.value("overlayIconIndex", overlayIconIndex);
		executeProcessIndex = json.value("executeProcessIndex", executeProcessIndex);
	}

	// データ出力
	inline void Save(nlohmann::json_abi_v3_12_0::json& json) const
	{
		json["name"]				= name;
		json["iconIndex"]			= iconIndex;
		json["colorX"]				= color.x;
		json["colorY"]				= color.y;
		json["colorZ"]				= color.z;
		json["colorW"]				= color.w;
		json["type"]				= static_cast<int>(type);
		json["isInPouch"]			= isInPouch;
		json["maxCountInpouch"]		= maxCountInpouch;
		json["rarity"]				= rarity;
		json["overlayIconIndex"]	= overlayIconIndex;
		json["executeProcessIndex"] = executeProcessIndex;
	}
};