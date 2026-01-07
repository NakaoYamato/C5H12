#pragma once

#include "../../Library/Resource/ResourceManager.h"
#include "QuestData.h"

class QuestManager : public ResourceBase
{
public:
	QuestManager() = default;
	~QuestManager() override {}

	// 初期化処理
	bool Initialize() override;
	// 名前取得
	std::string GetName() const override { return "QuestManager"; }
	// ファイルパス取得
	std::string GetFilePath() const override { return _filePath; };
	// ファイル読み込み
	bool LoadFromFile() override;
	// ファイル保存
	bool SaveToFile() override;

	// Gui描画
	void DrawGui() override;

	// クエストデータ取得
	std::vector<QuestData>& GetQuestDataList() { return _questDataList; }
	// クエストデータ取得
	QuestData* GetQuestData(size_t index);

private:
	std::string _filePath = "./Data/Resource/QuestManager.json";
	std::vector<QuestData> _questDataList;
};

// リソース設定
_REGISTER_RESOURCE(QuestManager)
