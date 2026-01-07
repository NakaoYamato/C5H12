#pragma once

#include "../../Library/Component/Component.h"

class QuestBoardController : public Component
{
public:
	QuestBoardController() = default;
	~QuestBoardController() override {}
	// 名前取得
	const char* GetName() const override { return "QuestBoardController"; }
	// 初期化処理
	void Start() override;
	// 更新処理
	void Update(float elapsedTime) override;
	// GUI描画処理
	void DrawGui() override;

	// 開く処理
	bool Open();
	// 開いているか
	bool IsOpen() const;
private:
};