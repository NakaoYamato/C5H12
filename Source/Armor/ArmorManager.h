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

	// 防具データ取得
	std::vector<ArmorData>& GetArmorDataList(ArmorType type) { return _armorDataMap[type]; }
	// 防具データ取得
	ArmorData* GetArmorData(ArmorType type, size_t index);

private:
	std::string _filePath = "./Data/Resource/ArmorManager.json";
	std::unordered_map<ArmorType, std::vector<ArmorData>> _armorDataMap;
};

// リソース設定
_REGISTER_RESOURCE(ArmorManager)
