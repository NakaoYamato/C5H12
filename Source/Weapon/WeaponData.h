#pragma once

#include "../../Library/Math/Vector.h"
#include "../../Library/Exporter/Exporter.h"

#include <string>
#include <vector>
#include <unordered_map>

/// <summary>
/// 武器の種類
/// </summary>
enum class WeaponType
{
	GreatSword,

	WeaponTypeMax,
};

// 斬れ味ゲージの最大値
static constexpr float WeaponSharpnessGaugeMax = 450.0f;
// 斬れ味レベルの最大値
static constexpr int WeaponSharpnessLevelMax = 7;

// 武器データ
struct WeaponData
{
	WeaponType	type = WeaponType::GreatSword;	// 武器の種類
	std::string name = "";						// 武器の名前
	std::string modelFilePath = "";				// モデルファイルのパス
	float		attack = 0.0f;					// 攻撃力
	int			rarity = 0;						// レア度
	std::vector<float> sharpnessGauge;			// 斬れ味ゲージ

	// レア度から色取得
	static Vector4 GetRarityColor(int rarity);
	// 斬れ味から色取得
	static Vector4 GetSharpnessColor(int sharpnessLevel);

	// GUI描画
	void DrawGui();
	// 斬れ味ゲージGUI描画
	static void DrawSharpnessGaugeGui(std::vector<float>& sharpnessGauge, bool canEdit);
	// データ保存
	void Load(nlohmann::json_abi_v3_12_0::json& json);
	// データ出力
	void Save(nlohmann::json_abi_v3_12_0::json& json) const;
};