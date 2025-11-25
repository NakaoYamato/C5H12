#pragma once

#include "../../Source/Skill/SkillData.h"

#include "../../Library/Math/Vector.h"
#include "../../Library/Exporter/Exporter.h"

#include <string>
#include <vector>
#include <unordered_map>

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
	std::unordered_map<SkillType, int> skills;// スキルデータ

	// レア度からカラー取得
	static Vector4 GetRarityColor(int rarity);
	// GUI描画
	void DrawGui();

	// データ保存
	void Load(nlohmann::json_abi_v3_12_0::json& json);
	// データ出力
	void Save(nlohmann::json_abi_v3_12_0::json& json) const;
};