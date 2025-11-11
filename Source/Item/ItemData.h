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
	std::wstring	textureFilePath = L"";					// アイテムのテクスチャファイルのパス
	Vector4			color			= Vector4::White;		// アイテムの色
	ItemType		type			= ItemType::Drinkable;	// アイテムの種類
	int				rarity			= 0;					// レア度
};