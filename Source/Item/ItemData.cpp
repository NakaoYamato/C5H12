#include "ItemData.h"

#include <Mygui.h>

// GUI描画
void ItemData::DrawGui(int itemIconTextureIndex)
{
	static std::vector<char> ItemTypeNames;
	if (ItemTypeNames.empty())
	{
		for (size_t i = 0; i < static_cast<size_t>(ItemType::ItemTypeMax); ++i)
		{
			for (char c : ToString<ItemType>(i))
			{
				ItemTypeNames.push_back(c);
			}
			ItemTypeNames.push_back('\0');
		}
	}

	ImGui::SetNextItemWidth(200.0f);
	ImGui::InputText(u8"名前", &name);

	ImGui::InputTextMultiline(u8"説明", &description, ImVec2(200.0f, 100.0f));

	ImGui::SetNextItemWidth(100.0f);
	if (ImGui::InputInt(u8"アイコン番号", &iconIndex))
		iconIndex = std::clamp<int>(iconIndex, -1, itemIconTextureIndex - 1);
	ImGui::SameLine();
	ImGui::SetNextItemWidth(100.0f);
	if (ImGui::InputInt(u8"オーバーレイアイコン番号", &overlayIconIndex))
		overlayIconIndex = std::clamp<int>(overlayIconIndex, -1, 7);

	ImGui::ColorEdit4(u8"表示色", &color.x, ImGuiColorEditFlags_NoInputs);

	ImGui::Combo(u8"種類", reinterpret_cast<int*>(&type), ItemTypeNames.data(), static_cast<int>(ItemType::ItemTypeMax));

	ImGui::SetNextItemWidth(100.0f);
	ImGui::Checkbox(u8"ポーチ内に入るかどうか", &isInPouch);
	ImGui::SetNextItemWidth(100.0f);
	ImGui::SameLine();
	ImGui::InputInt(u8"ポーチ内最大所持数", &maxCountInpouch);
	maxCountInpouch = std::clamp<int>(maxCountInpouch, -1, 99);

	ImGui::SetNextItemWidth(100.0f);
	ImGui::InputInt(u8"レア度", &rarity);
}

// データ保存
void ItemData::Load(nlohmann::json_abi_v3_12_0::json& json)
{
	name = json.value("name", name);
	description = json.value("description", description);
	iconIndex = json.value("iconIndex", iconIndex);
	color.x = json.value("colorX", color.x);
	color.y = json.value("colorY", color.y);
	color.z = json.value("colorZ", color.z);
	color.w = json.value("colorW", color.w);
	type = static_cast<ItemType>(json.value("type", static_cast<int>(type)));
	isInPouch = json.value("isInPouch", isInPouch);
	maxCountInpouch = json.value("maxCountInpouch", maxCountInpouch);
	rarity = json.value("rarity", rarity);
	overlayIconIndex = json.value("overlayIconIndex", overlayIconIndex);
	executeProcessIndex = json.value("executeProcessIndex", executeProcessIndex);
	size_t parametersSize = json.value("parametersSize", 0);
	for (size_t i = 0; i < parametersSize; ++i)
	{
		auto& sub = json["parameter" + std::to_string(i)];
		std::string parmName = sub.value("name", "");
		std::string type = sub.value("type", "");
		if (type == "int")
		{
			int value = sub.value("value", 0);
			parameters[parmName] = value;
		}
		else if (type == "float")
		{
			float value = sub.value("value", 0.0f);
			parameters[parmName] = value;
		}
		else if (type == "Vector2")
		{
			Vector2 value;
			value.x = sub.value("value0", 0.0f);
			value.y = sub.value("value1", 0.0f);
			parameters[parmName] = value;
		}
		else if (type == "Vector3")
		{
			Vector3 value;
			value.x = sub.value("value0", 0.0f);
			value.y = sub.value("value1", 0.0f);
			value.z = sub.value("value2", 0.0f);
			parameters[parmName] = value;
		}
		else if (type == "Vector4")
		{
			Vector4 value;
			value.x = sub.value("value0", 0.0f);
			value.y = sub.value("value1", 0.0f);
			value.z = sub.value("value2", 0.0f);
			value.w = sub.value("value3", 0.0f);
			parameters[parmName] = value;
		}
	}
}

// データ出力
void ItemData::Save(nlohmann::json_abi_v3_12_0::json& json) const
{
	json["name"]				= name;
	json["description"]			= description;
	json["iconIndex"]			= iconIndex;
	json["colorX"]				= color.x;
	json["colorY"]				= color.y;
	json["colorZ"]				= color.z;
	json["colorW"]				= color.w;
	json["type"]				= static_cast<int>(type);
	json["isInPouch"]			= isInPouch;
	json["maxCountInpouch"]		= maxCountInpouch;
	json["rarity"]				= rarity;
	json["overlayIconIndex"]	= overlayIconIndex;
	json["executeProcessIndex"] = executeProcessIndex;
	json["parametersSize"]		= parameters.size();
	int index = 0;
	for (auto& [parmName, parm] : parameters)
	{
		auto& sub	= json["parameter" + std::to_string(index)];
		sub["name"] = parmName;
		if (auto p = std::get_if<int>(&parm))
		{
			sub["type"]		= "int";
			sub["value"]	= *p;
		}
		else if (auto p = std::get_if<float>(&parm))
		{
			sub["type"]		= "float";
			sub["value"]	= *p;
		}
		else if (auto p = std::get_if<Vector2>(&parm))
		{
			sub["type"]		= "Vector2";
			sub["value0"]	= p->x;
			sub["value1"]	= p->y;
		}
		else if (auto p = std::get_if<Vector3>(&parm))
		{
			sub["type"]		= "Vector3";
			sub["value0"]	= p->x;
			sub["value1"]	= p->y;
			sub["value2"]	= p->z;
		}
		else if (auto p = std::get_if<Vector4>(&parm))
		{
			sub["type"]		= "Vector4";
			sub["value0"]	= p->x;
			sub["value1"]	= p->y;
			sub["value2"]	= p->z;
			sub["value3"]	= p->w;
		}
		index++;
	}
}
