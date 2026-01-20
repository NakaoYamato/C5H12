#pragma once

#include "../../Library/Actor/UI/Menu/MenuUIActor.h"
#include "../../Source/Quest/QuestOrderController.h"

class QuestBoardUIActor : public MenuUIActor
{
public:
	QuestBoardUIActor() = default;
	~QuestBoardUIActor() override {}
	// 生成時処理
	void OnCreate() override;

	// クエスト受注
	// 成功でtrue
	bool AcceptQuest(int questIndex);
private:
	std::weak_ptr<QuestOrderController> _questOrderController;
};

// クエスト掲示板UIウィジェット
class QuestBoardUIWidget : public MenuWidget
{
public:
	QuestBoardUIWidget(std::string name);
	~QuestBoardUIWidget() override {}

	// 描画処理
	void Render(const RenderContext& rc, MenuUIActor* owner) override;

	// 選択肢追加
	void AddOption(const std::string& label,
		const std::string& onSelectedCallbackName = "",
		const std::string& nextWidgetName = "") override
	{
		// 何もしない
	}
	// 選択肢選択処理
	void SelectOption(MenuUIActor* owner) override;
	// インデックス範囲制限
	size_t ClampSelectedOptionIndex(size_t index) override;

	void SetDisplayQuestType(QuestData::QuestType type) { _displayQuestType = type; }
protected:
	// クエストデータマネージャー
	std::weak_ptr<QuestManager> _questManager;

	// 描画するクエスト種類
	QuestData::QuestType _displayQuestType = QuestData::QuestType::Normal;
};