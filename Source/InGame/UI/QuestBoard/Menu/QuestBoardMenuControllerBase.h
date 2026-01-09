#pragma once

#include "../../Library/Component/SpriteRenderer.h"

class QuestBoardMenuControllerBase : public Component
{
public:
	QuestBoardMenuControllerBase() {}
	~QuestBoardMenuControllerBase() override {}

	// インデックス追加
	// direction
	// 0b0001:up 
	// 0b0010:down 
	// 0b0100:left 
	// 0b1000:right
	virtual void AddIndex(int direction) = 0;
	// リセット
	virtual void Reset() = 0;
	// 次へ進む
	virtual void NextState() = 0;
	// 前の状態へ戻る
	// メニューを閉じる場合はtrueを返す
	virtual bool PreviousState() = 0;
};