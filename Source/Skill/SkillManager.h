#pragma once

#include "../../Library/Resource/ResourceManager.h"
#include "SkillData.h"

#include <unordered_map>

class SkillManager : public ResourceBase
{
public:
	SkillManager() = default;
	~SkillManager() override {}

	// 初期化処理
	bool Initialize() override;

	// 名前取得
	std::string GetName() const override { return "SkillManager"; }
	// ファイルパス取得
	std::string GetFilePath() const override { return _filePath; };
	// ファイル読み込み
	bool LoadFromFile() override;
	// ファイル保存
	bool SaveToFile() override;
	// Gui描画
	void DrawGui() override;

	// スキルデータ取得
	std::unordered_map<SkillType, SkillData>& GetSkillDataMap() { return _skillDataMap; }
	// スキルデータ取得
	SkillData* GetSkillData(size_t index);

private:
	std::string _filePath = "./Data/Resource/SkillManager.json";
	std::unordered_map<SkillType, SkillData> _skillDataMap;
};

// リソース設定
_REGISTER_RESOURCE(SkillManager)
