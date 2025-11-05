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
	std::string name;						// 防具の名前
	std::string modelFilePath;				// モデルファイルのパス
	std::vector<std::string> hiddenNodes;	// 非表示にするノードの名前リスト
	ArmorType type;							// 防具の種類
	float defense;							// 防御力
};