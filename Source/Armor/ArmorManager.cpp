#include "ArmorManager.h"

#include "../../Library/Exporter/Exporter.h"

#include <Mygui.h>

// ファイル読み込み
bool ArmorManager::LoadFromFile()
{
	return false;
}

// ファイル保存
bool ArmorManager::SaveToFile()
{
	return false;
}

// Gui描画
void ArmorManager::DrawGui()
{
	static const char* ArmorTypeNames[] =
	{
		"Head",
		"Chest",
		"Arm",
		"Waist",
		"Leg"
	};

	if (ImGui::Button("Add"))
	{
		_armorDataList.push_back(ArmorData());
	}
	ImGui::Separator();

	int index = 0;
	for (auto& data : _armorDataList)
	{
		if (ImGui::TreeNode(std::to_string(index).c_str()))
		{
			ImGui::InputText("Name", &data.name);
			ImGui::Text("Model File Path:");
			ImGui::SameLine();
			ImGui::Text(data.modelFilePath.c_str());
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
						data.modelFilePath = path.u8string();
					}
					catch (...)
					{
						data.modelFilePath = filepath;
					}
				}
			}
			ImGui::InputFloat("Defense", &data.defense);
			ImGui::InputInt("Rarity", &data.rarity);
			int typeIndex = static_cast<int>(data.type);
			if (ImGui::Combo("Type", &typeIndex, ArmorTypeNames, _countof(ArmorTypeNames)))
				data.type = static_cast<ArmorType>(typeIndex);
			ImGui::TreePop();
		}
	}
}
