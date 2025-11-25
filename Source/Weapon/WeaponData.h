#pragma once

#include "../../Library/Math/Vector.h"
#include "../../Library/Exporter/Exporter.h"

#include <string>
#include <vector>
#include <unordered_map>

enum class WeaponType
{
	GreatSword,

	WeaponTypeMax,
};

struct WeaponData
{
	WeaponType	type = WeaponType::GreatSword;	// 武器の種類
	std::string name = "";						// 武器の名前
	std::string modelFilePath = "";			// モデルファイルのパス
	float		attack = 0.0f;							// 攻撃力
	int			rarity = 0;								// レア度

	// レア度からカラー取得
	static Vector4 GetRarityColor(int rarity);
	// GUI描画
	void DrawGui();
	// データ保存
	void Load(nlohmann::json_abi_v3_12_0::json& json);
	// データ出力
	void Save(nlohmann::json_abi_v3_12_0::json& json) const;
};