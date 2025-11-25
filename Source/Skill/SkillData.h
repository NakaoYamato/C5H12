#pragma once

#include "../../Library/Exporter/Exporter.h"
#include <string>

enum class SkillType
{
	AttackBoost,			// 攻撃力
	DefenseBoost,			// 防御力
	CriticalBoost,			// 会心率
	HealthBoost,			// 体力
	StaminaBoost,			// スタミナ

	SkillTypeMax
};

struct SkillData
{
	std::string name = "";				// スキルの名前
	std::string description = "";		// スキルの説明
	int			maxLevel = 1;			// スキルの最大レベル

	// GUI描画
	void DrawGui();
	// データ保存
	void Load(nlohmann::json_abi_v3_12_0::json& json);
	// データ出力
	void Save(nlohmann::json_abi_v3_12_0::json& json) const;
};