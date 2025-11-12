#pragma once

#include "../../Library/Math/Vector.h"

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
	int				rarity			= 0;					// レア度
};