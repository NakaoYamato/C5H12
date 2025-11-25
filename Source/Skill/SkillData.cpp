#include "SkillData.h"

#include <Mygui.h>

// GUI描画
void SkillData::DrawGui()
{
	ImGui::InputText(u8"名前", &name);
	ImGui::InputTextMultiline(u8"説明", &description, ImVec2(-1.0f, 100.0f));
	ImGui::InputInt(u8"最大レベル", &maxLevel);
}

// データ保存
void SkillData::Load(nlohmann::json_abi_v3_12_0::json& json)
{
	name = json.value("name", "");
	description = json.value("description", "");
	maxLevel = json.value("maxLevel", 1);
}

// データ出力
void SkillData::Save(nlohmann::json_abi_v3_12_0::json& json) const
{
	json["name"] = name;
	json["description"] = description;
	json["maxLevel"] = maxLevel;
}
