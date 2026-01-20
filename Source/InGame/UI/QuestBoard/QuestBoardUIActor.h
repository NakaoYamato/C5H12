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

	// 開始処理
	void OnEnter() override;
	// 描画処理
	void Render(const RenderContext& rc, MenuUIActor* owner) override;
	// GUI描画処理
	void DrawGui(MenuUIActor* owner) override;

	// 選択肢追加
	void AddOption(const std::string& label,
		const std::string& onSelectedCallbackName = "",
		const std::string& nextWidgetName = "") override
	{
		// 何もしない
	}
	// 選択肢選択処理
	void SelectOption(MenuUIActor* owner) override;
	// 戻る選択肢選択処理
	void BackOption(MenuUIActor* owner) override;
	// インデックス範囲制限
	size_t ClampSelectedOptionIndex(size_t index) override;

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

	// 説明文の画像
	Sprite _descriptionSprite;
	// 説明文描画位置
	Vector2 _descriptionLabelOffset{ 50.0f, 10.0f };
	// 説明文フォントサイズ
	Vector2 _descriptionFontSize{ 0.5f, 0.5f };
	// 説明文色
	Vector4 _descriptionLabelColor = Vector4::White;

	// クエスト受注確認テキスト位置
	Vector2 _confirmLabelOffset{ 50.0f, 200.0f };
	// クエスト受注確認テキストフォントサイズ
	Vector2 _confirmFontSize{ 0.7f, 0.7f };

	// クエストを選択確認フラグ
	bool _isConfirmingQuest = false;
};