#pragma once

#include "../../Library/Resource/ResourceManager.h"
#include "ArmorData.h"

class ArmorManager : public ResourceBase
{
public:
	ArmorManager() = default;
	~ArmorManager() override {}

	// 名前取得
	std::string GetName() const override { return "ArmorManager"; }
	// ファイルパス取得
	std::string GetFilePath() const override { return _filePath; };

	// ファイル読み込み
	bool LoadFromFile() override;
	// ファイル保存
	bool SaveToFile() override;

	// Gui描画
	void DrawGui() override;

private:
	std::string _filePath = "./Data/Resource/ArmorManager.json";
	std::vector<ArmorData> _armorDataList;
};

// リソース設定
_REGISTER_RESOURCE(ArmorManager)
