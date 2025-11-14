#pragma once

#include "../../Library/Math/Vector.h"
#include "../../Library/Exporter/Exporter.h"

#include <string>
#include <vector>
#include <Mygui.h>

enum class ArmorType
{
	Head,
	Chest,
	Arm,
	Waist,
	Leg
};

struct ArmorData
{
	std::string name = "";					// 防具の名前
	std::string modelFilePath = "";			// モデルファイルのパス
	std::vector<std::string> hiddenMeshes;	// 非表示にするメッシュの名前リスト
	ArmorType type = ArmorType::Head;		// 防具の種類
	float defense = 0.0f;					// 防御力
	int rarity = 0;							// レア度

	// レア度からカラー取得
	static Vector4 GetRarityColor(int rarity)
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

	inline void DrawGui()
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
		ImGui::InputFloat(u8"防御力", &defense);
		ImGui::InputInt(u8"レア度", &rarity);
		if (ImGui::TreeNode(u8"非表示メッシュ"))
		{
			size_t hiddenMeshIndex = 0;
			for (auto& meshName : hiddenMeshes)
			{
				ImGui::InputText((u8"非表示メッシュ" + std::to_string(hiddenMeshIndex)).c_str(), &meshName);
				ImGui::SameLine();
				if (ImGui::Button((u8"削除##hiddenMesh" + std::to_string(hiddenMeshIndex)).c_str()))
				{
					hiddenMeshes.erase(hiddenMeshes.begin() + hiddenMeshIndex);
					break;
				}
				hiddenMeshIndex++;
			}
			ImGui::TreePop();
		}
	}

	// データ保存
	inline void Load(nlohmann::json_abi_v3_12_0::json& json)
	{
		name			= json.value("name", name);
		modelFilePath	= json.value("modelFilePath", modelFilePath);
		hiddenMeshes.clear();
		if (json.contains("hiddenMeshesSize"))
		{
			size_t size = json["hiddenMeshesSize"].get<std::size_t>();
			for (size_t j = 0; j < size; ++j)
			{
				hiddenMeshes.push_back(json.value("hiddenMesh" + std::to_string(j), ""));
			}
		}
		type	= static_cast<ArmorType>(json.value("type", static_cast<int>(type)));
		defense = json.value("defense", defense);
		rarity	= json.value("rarity", rarity);
	} 

	// データ出力
	inline void Save(nlohmann::json_abi_v3_12_0::json& json) const
	{
		json["name"]				= name;
		json["modelFilePath"]		= modelFilePath;
		json["hiddenMeshesSize"]	= hiddenMeshes.size();
		for (size_t j = 0; j < hiddenMeshes.size(); ++j)
		{
			json["hiddenMesh" + std::to_string(j)] = hiddenMeshes[j];
		}
		json["type"]	= static_cast<int>(type);
		json["defense"] = defense;
		json["rarity"]	= rarity;
	}
};