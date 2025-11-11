#pragma once

#include "../../Library/Math/Vector.h"

#include <string>
#include <vector>

enum class ArmorType
{
	Head,
	Chest,
	Arm,
	Waist,
	Leg
};

struct ArmorData
{
	std::string name = "";					// 防具の名前
	std::string modelFilePath = "";			// モデルファイルのパス
	std::vector<std::string> hiddenMeshes;	// 非表示にするメッシュの名前リスト
	ArmorType type = ArmorType::Head;		// 防具の種類
	float defense = 0.0f;					// 防御力
	int rarity = 0;							// レア度

	// レア度からカラー取得
	static Vector4 GetRarityColor(int rarity)
	{
		switch (rarity)
		{
		case 1:
			return Vector4::Gray;
		case 2:
			return Vector4::White;
		case 3:
			return Vector4::LightGreen;
		case 4:
			return Vector4::Green;
		case 5:
			return Vector4::Cyan;
		case 6:
			return Vector4::Blue;
		case 7:
			return Vector4::Purple;
		case 8:
			return Vector4::Orange;
		default:
			return Vector4::White;
		}
	}
};