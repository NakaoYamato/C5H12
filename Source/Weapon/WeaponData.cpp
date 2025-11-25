#include "WeaponData.h"

#include "../../Library/Algorithm/Converter.h"

#include <Mygui.h>

// レア度からカラー取得
Vector4 WeaponData::GetRarityColor(int rarity)
{
	switch (rarity)
	{
	case 1:
		return Vector4::Gray;
	case 2:
		return Vector4::White;
	case 3:
		return Vector4::LightGreen;
	case 4:
		return Vector4::Green;
	case 5:
		return Vector4::Cyan;
	case 6:
		return Vector4::Blue;
	case 7:
		return Vector4::Purple;
	case 8:
		return Vector4::Orange;
	default:
		return Vector4::White;
	}
}

// GUI描画
void WeaponData::DrawGui()
{
	ImGui::InputText(u8"名前", &name);
	ImGui::Text(u8"ファイルパス:");
	ImGui::SameLine();
	ImGui::Text(modelFilePath.c_str());
	ImGui::SameLine();
	if (ImGui::Button("..."))
	{
		// ダイアログを開く
		std::string filepath;
		std::string currentDirectory;
		Debug::Dialog::DialogResult result = Debug::Dialog::OpenFileName(filepath, currentDirectory, ImGui::ModelFilter);
		// ファイルを選択したら
		if (result == Debug::Dialog::DialogResult::Yes || result == Debug::Dialog::DialogResult::OK)
		{
			try
			{
				// 相対パス取得
				std::filesystem::path path =
					std::filesystem::relative(filepath, currentDirectory);
				modelFilePath = path.u8string();
			}
			catch (...)
			{
				modelFilePath = filepath;
			}
		}
	}
	ImGui::InputFloat(u8"攻撃力", &attack);
	ImGui::InputInt(u8"レア度", &rarity);
}

// データ保存
void WeaponData::Load(nlohmann::json_abi_v3_12_0::json& json)
{
	type = static_cast<WeaponType>(json.value("type", static_cast<int>(type)));
	name = json.value("name", name);
	modelFilePath = json.value("modelFilePath", modelFilePath);
	attack = json.value("attack", attack);
	rarity = json.value("rarity", rarity);
}

// データ出力
void WeaponData::Save(nlohmann::json_abi_v3_12_0::json& json) const
{
	json["type"] = static_cast<int>(type);
	json["name"] = name;
	json["modelFilePath"] = modelFilePath;
	json["attack"] = attack;
	json["rarity"] = rarity;
}
