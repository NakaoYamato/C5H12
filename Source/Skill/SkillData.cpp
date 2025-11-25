#include "SkillData.h"

#include <Mygui.h>

// GUI描画
void SkillData::DrawGui()
{
	ImGui::InputText(u8"名前", &name);
	ImGui::InputTextMultiline(u8"説明", &description, ImVec2(-1.0f, 100.0f));
	ImGui::InputInt(u8"最大レベル", &maxLevel);
	ImGui::ColorEdit4(u8"スキルカラー", &color.x);
}

// データ保存
void SkillData::Load(nlohmann::json_abi_v3_12_0::json& json)
{
	name = json.value("name", "");
	description = json.value("description", "");
	maxLevel = json.value("maxLevel", 1);
	color.x = json.value("colorX", 1.0f);
	color.y = json.value("colorY", 1.0f);
	color.z = json.value("colorZ", 1.0f);
	color.w = json.value("colorW", 1.0f);
}

// データ出力
void SkillData::Save(nlohmann::json_abi_v3_12_0::json& json) const
{
	json["name"] = name;
	json["description"] = description;
	json["maxLevel"] = maxLevel;
	json["colorX"] = color.x;
	json["colorY"] = color.y;
	json["colorZ"] = color.z;
	json["colorW"] = color.w;
}
