#pragma once

#include "../../Library/Math/Vector.h"
#include "../../Library/Exporter/Exporter.h"
#include "../../Library/Algorithm/Converter.h"

#include <string>
#include <vector>

struct QuestData
{
	enum class QuestType
	{
		Normal,
		Event,

		QuestTypeMax
	};

	enum class TargetType
	{
		// 狩猟
		Hunt,
		// 討伐
		Extermination,
		// 捕獲
		Capture,
		// 採取
		Gather,
		// 配達
		Delivery,

		TargetTypeMax
	};

	// ターゲットデータ
	struct TargetData
	{
		std::string name	= "";	// ターゲットの名前
		int count			= 1;	// ターゲットの数
		int spawnAreaIndex	= -1;	// ターゲットのスポーンエリアインデックス
	};

	std::string				name		= "";	// クエストの名前
	std::string				description = "";	// クエストの説明
	QuestType 				questType	= QuestType::Normal;// クエストの種類
	TargetType 				targetType	= TargetType::Hunt;	// ターゲットの種類
	std::vector<TargetData> targets;			// クエストターゲットリスト
	int						stageIndex = 0;		// クエストステージインデックス
	int						level		= 1;	// クエストのレベル
	int						rewardGold	= 0;	// クエスト報酬ゴールド
	int						respawnLimit = 2;	// リスポーン可能回数
	float 					timeLimit = 0.0f;	// クエスト制限時間(s) 0.0fで無制限

	// GUI描画
	void DrawGui();
	// データ保存
	void Load(nlohmann::json_abi_v3_12_0::json& json);
	// データ出力
	void Save(nlohmann::json_abi_v3_12_0::json& json) const;
};