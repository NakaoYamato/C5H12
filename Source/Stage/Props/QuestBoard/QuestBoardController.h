#pragma once

#include "../../Library/Component/Component.h"
#include "../../Source/InGame/Input/QuestBoard/QuestBoardInput.h"
#include "../../Source/Common/InteractionController.h"

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

private:
	std::weak_ptr<QuestBoardInput> _questBoardInput;
	std::weak_ptr<InteractionController> _interactionController;

	// 選択UIのオフセット
	Vector3 _selectUIOffset = Vector3(0.0f, 1.5f, 0.0f);
};