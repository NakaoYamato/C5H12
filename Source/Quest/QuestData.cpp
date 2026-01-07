#include "QuestData.h"

#include <Mygui.h>

// GUI描画
void QuestData::DrawGui()
{
	ImGui::SetNextItemWidth(200.0f);
	ImGui::InputText(u8"名前", &name);
	ImGui::InputTextMultiline(u8"説明", &description, ImVec2(200.0f, 100.0f));
	ImGui::Separator();

	ImGui::SetNextItemWidth(100.0f);
	ImGui::InputInt(u8"ステージ", &stageIndex);
	ImGui::SetNextItemWidth(100.0f);
	ImGui::InputInt(u8"レベル", &level);
	level = std::clamp<int>(level, 1, 999);
	ImGui::SetNextItemWidth(100.0f);
	ImGui::InputInt(u8"報酬ゴールド", &rewardGold);
	ImGui::SetNextItemWidth(100.0f);
	ImGui::InputInt(u8"リスポーン可能回数", &respawnLimit);
	ImGui::SetNextItemWidth(100.0f);
	ImGui::DragFloat(u8"制限時間(s)(0.0で無制限)", &timeLimit, 1.0f, 0.0f, 60.0f);
	ImGui::Separator();

	if (ImGui::Button(u8"ターゲット追加"))
	{
		targets.emplace_back();
	}
	for (size_t i = 0; i < targets.size(); ++i)
	{
		auto& target = targets[i];
		if (ImGui::TreeNode((u8"ターゲット" + std::to_string(i)).c_str()))
		{
			ImGui::SetNextItemWidth(100.0f);
			ImGui::InputInt(u8"ターゲットインデックス", &target.index);
			ImGui::SetNextItemWidth(100.0f);
			ImGui::InputInt(u8"ターゲット数", &target.count);
			target.count = std::clamp<int>(target.count, 1, 999);
			ImGui::SetNextItemWidth(100.0f);
			ImGui::InputInt(u8"スポーンエリアインデックス", &target.spawnAreaIndex);

			if (ImGui::Button(u8"ターゲット削除"))
			{
				targets.erase(targets.begin() + i);
				ImGui::TreePop();
				break;
			}

			ImGui::TreePop();
		}
	}
}

// データ保存
void QuestData::Load(nlohmann::json_abi_v3_12_0::json& json)
{
	name			= json.value("name", name);
	description		= json.value("description", description);
	stageIndex		= json.value("stageIndex", stageIndex);
	level			= json.value("level", level);
	rewardGold		= json.value("rewardGold", rewardGold);
	respawnLimit	= json.value("respawnLimit", respawnLimit);
	timeLimit		= json.value("timeLimit", timeLimit);
	targets.clear();
	size_t targetsSize = json.value("targetsSize", 0);
	for (size_t i = 0; i < targetsSize; ++i)
	{
		auto& sub = json["target" + std::to_string(i)];
		TargetData target;
		target.index			= sub.value("index", -1);
		target.count			= sub.value("count", 1);
		target.spawnAreaIndex	= sub.value("spawnAreaIndex", -1);
		targets.push_back(target);
	}
}

// データ出力
void QuestData::Save(nlohmann::json_abi_v3_12_0::json& json) const
{
	json["name"]			= name;
	json["description"]		= description;
	json["stageIndex"]		= stageIndex;
	json["level"]			= level;
	json["rewardGold"]		= rewardGold;
	json["respawnLimit"]	= respawnLimit;
	json["timeLimit"]		= timeLimit;
	json["targetsSize"]		= targets.size();
	for (size_t i = 0; i < targets.size(); ++i)
	{
		auto& target = targets[i];
		auto& sub = json["target" + std::to_string(i)];
		sub["index"]			= target.index;
		sub["count"]			= target.count;
		sub["spawnAreaIndex"]	= target.spawnAreaIndex;
	}
}
