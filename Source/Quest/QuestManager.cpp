#include "QuestManager.h"

#include <Mygui.h>

// 初期化処理
bool QuestManager::Initialize()
{
	return false;
}

// ファイル読み込み
bool QuestManager::LoadFromFile()
{
	nlohmann::json jsonData;
	if (Exporter::LoadJsonFile(_filePath, &jsonData))
	{
		_questDataList.clear();
		if (!jsonData.contains("QuestDataListSize"))
			return false;

		size_t size = jsonData["QuestDataListSize"].get<std::size_t>();
		for (size_t i = 0; i < size; ++i)
		{
			auto& sub = jsonData["QuestDataList" + std::to_string(i)];
			QuestData data;
			data.Load(sub);
			_questDataList.push_back(data);
		}
		return true;
	}
	return false;
}

// ファイル保存
bool QuestManager::SaveToFile()
{
	nlohmann::json jsonData;
	jsonData["QuestDataListSize"] = _questDataList.size();
	for (size_t i = 0; i < _questDataList.size(); ++i)
	{
		auto& sub = jsonData["QuestDataList" + std::to_string(i)];
		_questDataList[i].Save(sub);
	}
	return Exporter::SaveJsonFile(_filePath, jsonData);
}

// Gui描画
void QuestManager::DrawGui()
{
	if (ImGui::Button(u8"クエスト追加"))
	{
		_questDataList.emplace_back();
	}
	ImGui::Separator();
	for (size_t i = 0; i < _questDataList.size(); ++i)
	{
		auto& data = _questDataList[i];
		if (ImGui::TreeNode(std::to_string(i).c_str()))
		{
			data.DrawGui();
			ImGui::TreePop();
		}
	}
}

// クエストデータ取得
QuestData* QuestManager::GetQuestData(size_t index)
{
	if (index < _questDataList.size())
	{
		return &_questDataList[index];
	}
	return nullptr;
}
