#pragma once

#include "../../Source/InGame/InputManager.h"
#include "../../UI/QuestQueue/QuestQueueUIController.h"

class QuestQueueInput : public InputControllerBase
{
public:
	QuestQueueInput() {}
	~QuestQueueInput() override {}
	// 名前取得
	const char* GetName() const override { return "QuestQueueInput"; }
	// 開始処理
	void Start() override;
	// GUI描画
	void DrawGui() override;
	// 起動時関数
	void OnEntry() override;
	// 終了時関数
	void OnExit() override;
	// 更新時処理
	void OnUpdate(float elapsedTime)  override;

private:
	std::weak_ptr<QuestQueueUIController> _questQueueUIController;
};