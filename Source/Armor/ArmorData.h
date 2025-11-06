#pragma once

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
};