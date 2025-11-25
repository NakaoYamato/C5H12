#include "SkillManager.h"

#include "../../Library/Exporter/Exporter.h"
#include "../../Library/Algorithm/Converter.h"

#include <Mygui.h>

// 初期化処理
bool SkillManager::Initialize()
{
	size_t skillTypeMax = static_cast<size_t>(SkillType::SkillTypeMax);
	for (size_t i = 0; i < skillTypeMax; ++i)
	{
		if (_skillDataMap.find(static_cast<SkillType>(i)) == _skillDataMap.end())
		{
			SkillData data;
			_skillDataMap[static_cast<SkillType>(i)] = data;
		}
	}

	return true;
}

// ファイル読み込み
bool SkillManager::LoadFromFile()
{
	nlohmann::json jsonData;
	if (Exporter::LoadJsonFile(_filePath, &jsonData))
	{
		_skillDataMap.clear();
		size_t skillTypeMax = static_cast<size_t>(SkillType::SkillTypeMax);
		for (size_t i = 0; i < skillTypeMax; ++i)
		{
			std::string typeName = ToString<SkillType>(i);
			if (!jsonData.contains(typeName))
				continue;
			auto& sub = jsonData[typeName];
			SkillData data;
			data.Load(sub);
			_skillDataMap[static_cast<SkillType>(i)] = data;
		}
		return true;
	}
	return false;
}

// ファイル保存
bool SkillManager::SaveToFile()
{
	nlohmann::json jsonData;
	for (auto& [type, data] : _skillDataMap)
	{
		std::string typeName = ToString<SkillType>(static_cast<size_t>(type));
		auto& sub = jsonData[typeName];
		data.Save(sub);
	}
	return Exporter::SaveJsonFile(_filePath, jsonData);
}

// Gui描画
void SkillManager::DrawGui()
{
	for (auto& [type, data] : _skillDataMap)
	{
		if (ImGui::TreeNode(ToString<SkillType>(static_cast<size_t>(type)).c_str()))
		{
			data.DrawGui();
			ImGui::TreePop();
		}
	}
}

// スキルデータ取得
SkillData* SkillManager::GetSkillData(size_t index)
{
	size_t skillTypeMax = static_cast<size_t>(SkillType::SkillTypeMax);
	if (index >= skillTypeMax)
		return nullptr;
	SkillType type = static_cast<SkillType>(index);
	auto it = _skillDataMap.find(type);
	if (it != _skillDataMap.end())
	{
		return &it->second;
	}
	return nullptr;
}
