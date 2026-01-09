#pragma once

#include "QuestBoardMenuControllerBase.h"
#include "../../Library/Component/SpriteRenderer.h"

class QuestBoardSelectMenuController : public QuestBoardMenuControllerBase
{
public:
	enum class SelectMenuOption
	{
		NewOrderOption,
		AcceptOption,

		MaxOption,
	};

public:
	QuestBoardSelectMenuController() {}
	~QuestBoardSelectMenuController() override {}
	// 名前取得
	const char* GetName() const override { return "QuestBoardSelectMenuController"; }

	// 開始処理
	void Start() override;
	// GUI描画
	void DrawGui() override;

	// インデックス追加
	// direction
	// 0b0001:up 
	// 0b0010:down 
	// 0b0100:left 
	// 0b1000:right
	void AddIndex(int direction) override;
	// リセット
	void Reset() override;
	// 次へ進む
	void NextState() override {}
	// 前の状態へ戻る
	// メニューを閉じる場合はtrueを返す
	bool PreviousState() override;

	SelectMenuOption GetSelectIndex() const { return _index; }

private:
	std::weak_ptr<SpriteRenderer> _spriteRenderer;

	// 各種スプライトの名前
	const std::string NewOrder = "NewOrder";
	const std::string AcceptMenu = "AcceptMenu";
	const std::string FrontSpr = "Front";

	// 選択肢インデックス
	SelectMenuOption _index = SelectMenuOption::NewOrderOption;
};