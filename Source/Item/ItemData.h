#pragma once

#include "../../Library/Math/Vector.h"
#include "../../Library/Exporter/Exporter.h"
#include "../../Library/Algorithm/Converter.h"

#include <string>
#include <vector>
#include <variant>

enum class ItemType
{
	Drinkable,
	Useable,
	Installable,
	GrindingStone,

	Material,

	ItemTypeMax,
};

struct ItemData
{
	using VariantType = std::variant<int, float, Vector2, Vector3, Vector4>;
	using ParameterMap = std::unordered_map<std::string, VariantType>;

	std::string		name			= "";					// アイテムの名前
	std::string 	description		= "";					// アイテムの説明
	int 			iconIndex		= -1;					// アイテムアイコンの番号
	Vector4			color			= Vector4::White;		// アイテムの色
	ItemType		type			= ItemType::Drinkable;	// アイテムの種類
	bool			isInPouch		= true;					// ポーチ内に入るかどうか
	int				maxCountInpouch = 99;					// ポーチ内最大所持数 -1は無限
	int				rarity			= 0;					// レア度
	int				overlayIconIndex = -1;					// アイテムオーバーレイアイコンの番号
	int 			executeProcessIndex = -1;				// アイテム実行処理のインデックス
	ParameterMap	parameters;								// アイテム実行処理のパラメータマップ

	// GUI描画
	void DrawGui(int itemIconTextureIndex);

	// データ保存
	void Load(nlohmann::json_abi_v3_12_0::json& json);

	// データ出力
	void Save(nlohmann::json_abi_v3_12_0::json& json) const;
};