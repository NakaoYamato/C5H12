#pragma once

#include "../../Library/Math/Vector.h"
#include "../../Library/Exporter/Exporter.h"

#include <string>
#include <vector>

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
	int				maxCountInpouch = 99;					// ポーチ内最大所持数
	int				rarity			= 0;					// レア度
	int				overlayIconIndex = -1;					// アイテムオーバーレイアイコンの番号
	int 			executeProcessIndex = -1;				// アイテム実行処理のインデックス

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
		maxCountInpouch = json.value("maxCountInpouch", maxCountInpouch);
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
		json["maxCountInpouch"]		= maxCountInpouch;
		json["rarity"]				= rarity;
		json["overlayIconIndex"]	= overlayIconIndex;
		json["executeProcessIndex"] = executeProcessIndex;
	}
};