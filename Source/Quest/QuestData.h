#pragma once

#include "../../Library/Math/Vector.h"
#include "../../Library/Exporter/Exporter.h"
#include "../../Library/Algorithm/Converter.h"

#include <string>
#include <vector>

struct QuestData
{
	struct TargetData
	{
		int index			= -1;	// ターゲットのインデックス
		int count			= 1;	// ターゲットの数
		int spawnAreaIndex	= -1;	// ターゲットのスポーンエリアインデックス
	};

	std::string				name		= "";	// クエストの名前
	std::string				description = "";	// クエストの説明
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