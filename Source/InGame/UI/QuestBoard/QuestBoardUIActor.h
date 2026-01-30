#pragma once

#include "../../Library/Actor/UI/Menu/MenuUIActor.h"
#include "../../Source/Quest/QuestController.h"

class QuestBoardUIActor : public MenuUIActor
{
public:
	QuestBoardUIActor() = default;
	~QuestBoardUIActor() override {}
	// 生成時処理
	void OnCreate() override;
	// 起動フラグが変化したときの処理
	void OnChangedActive(bool isActive) override;

	// クエスト受注
	// 成功でtrue
	bool AcceptQuest(int questIndex);

private:
	std::weak_ptr<QuestController> _questController;
};

// クエスト掲示板UIウィジェット
class QuestBoardUIWidget : public MenuWidget
{
public:
	QuestBoardUIWidget(std::string name);
	~QuestBoardUIWidget() override {}

	// ウィジェット名取得
	virtual std::string GetWidgetName() const { return ClassToString<QuestBoardUIWidget>(); }

	// 開始処理
	void OnEnter() override;
	// 描画処理
	void Render(const RenderContext& rc, MenuUIActor* owner) override;
	// GUI描画処理
	void DrawGui(MenuUIActor* owner) override;

	// 選択肢追加
	void AddOption(const std::string& label,
		const std::string& nextWidgetName = "",
		const std::string& canSelectCallbackName = "",
		const std::string& onSelectedCallbackName = "") override
	{
		// 何もしない
	}
	// 選択肢選択処理
	void SelectOption(MenuUIActor* owner) override;
	// 戻る選択肢選択処理
	void BackOption(MenuUIActor* owner) override;
	// インデックス範囲制限
	int ClampSelectedOptionIndex(int index) override;

	void SetDisplayQuestType(QuestData::QuestType type) { _displayQuestType = type; }

#pragma region ファイル
	// ファイル読み込み処理
	void OnLoadFromFile(nlohmann::json* json) override;
	// ファイル保存処理
	void OnSaveToFile(nlohmann::json* json) override;
#pragma endregion

protected:
	// クエストデータマネージャー
	std::weak_ptr<QuestManager> _questManager;

	// 描画するクエスト種類
	QuestData::QuestType _displayQuestType = QuestData::QuestType::Normal;

	// クエスト受注確認テキスト位置
	Vector2 _confirmLabelOffset{ 50.0f, 200.0f };
	// クエスト受注確認テキストフォントサイズ
	Vector2 _confirmFontSize{ 0.7f, 0.7f };

	// クエストを選択確認フラグ
	bool _isConfirmingQuest = false;
};

// クエスト中のクエスト掲示板UIウィジェット
class InQuestStatusUIWidget : public MenuWidget
{
public:
	InQuestStatusUIWidget(std::string name) : MenuWidget(name) {}
	~InQuestStatusUIWidget() override {}
	// 開始処理
	void OnEnter() override;
	// 描画処理
	void Render(const RenderContext& rc, MenuUIActor* owner) override;
	// GUI描画処理
	void DrawGui(MenuUIActor* owner) override;

#pragma region ファイル
	// ファイル読み込み処理
	void OnLoadFromFile(nlohmann::json* json) override;
	// ファイル保存処理
	void OnSaveToFile(nlohmann::json* json) override;
#pragma endregion

	void SetQuestController(std::weak_ptr<QuestController> controller)
	{
		_questController = controller;
	}
private:
	// クエスト受注コントローラー
	std::weak_ptr<QuestController> _questController;

	// クエストタイトル
	std::string _questTitle{};
	// クエストタイトル位置
	Vector2 _questTitleOffset{ 50.0f, 50.0f };
	// クエストタイトルフォントサイズ
	Vector2 _questTitleFontSize{ 1.0f, 1.0f };

	// クエスト説明文
	std::string _questDescription{};
	// クエスト説明文フォントサイズ
	Vector2 _questDescriptionFontSize{ 0.7f, 0.7f };
	// クエスト説明文位置
	Vector2 _questDescriptionOffset{ 50.0f, 120.0f };
};