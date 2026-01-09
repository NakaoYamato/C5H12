#pragma once

#include "QuestBoardMenuControllerBase.h"
#include "../../Library/Component/SpriteRenderer.h"

class QuestBoardNewOrderMenuController : public QuestBoardMenuControllerBase
{
public:
	QuestBoardNewOrderMenuController() {}
	~QuestBoardNewOrderMenuController() override {}
	// 名前取得
	const char* GetName() const override { return "QuestBoardNewOrderMenuController"; }

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
	void NextState() override;
	// 前の状態へ戻る
	// メニューを閉じる場合はtrueを返す
	bool PreviousState() override;
private:
	std::weak_ptr<SpriteRenderer> _spriteRenderer;
};