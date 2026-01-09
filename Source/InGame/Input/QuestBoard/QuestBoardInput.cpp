#include "QuestBoardInput.h"

#include "../../Library/Scene/Scene.h"
#include "../../Source/InGame/InGameCanvasActor.h"

#include <imgui.h>

// 開始処理
void QuestBoardInput::Start()
{
	// カメラは動かせない
	SetCanMoveCamera(false);

	// キャンバスからUIを取得
	if (auto canvas = GetActor()->GetScene()->GetActorManager().FindByClass<InGameCanvasActor>(ActorTag::UI))
	{
		for (auto& child : canvas->GetChildren())
		{
			if (auto questBoardUIController = child->GetComponent<QuestBoardUIController>())
			{
				_questBoardUIController = questBoardUIController;
				questBoardUIController->Close();
				break;
			}
		}
	}
}

// GUI描画
void QuestBoardInput::DrawGui()
{
}

// 起動時関数
void QuestBoardInput::OnEntry()
{
	_questBoardUIController.lock()->Open();
}

// 終了時関数
void QuestBoardInput::OnExit()
{
	_questBoardUIController.lock()->Close();
}

// 更新時処理
void QuestBoardInput::OnUpdate(float elapsedTime)
{
	auto questBoardUIController = _questBoardUIController.lock();
	if (!questBoardUIController)
		return;

	unsigned int currentInputDirection = 0;
	if (_INPUT_REPEAT("Up"))
		currentInputDirection |= static_cast<unsigned int>(QuestBoardUIController::InputDirection::Up);
	if (_INPUT_REPEAT("Down"))
		currentInputDirection |= static_cast<unsigned int>(QuestBoardUIController::InputDirection::Down);
	if (_INPUT_REPEAT("Left"))
		currentInputDirection |= static_cast<unsigned int>(QuestBoardUIController::InputDirection::Left);
	if (_INPUT_REPEAT("Right"))
		currentInputDirection |= static_cast<unsigned int>(QuestBoardUIController::InputDirection::Right);
	questBoardUIController->SetInputDirection(currentInputDirection);

	if (_INPUT_TRIGGERD("Select"))
		questBoardUIController->SetInputState(QuestBoardUIController::InputState::Select);
	if (_INPUT_TRIGGERD("Back") || _INPUT_TRIGGERD("Menu"))
		questBoardUIController->SetInputState(QuestBoardUIController::InputState::Back);
	if (_INPUT_TRIGGERD("L3"))
		questBoardUIController->SetInputState(QuestBoardUIController::InputState::L3);
	if (_INPUT_TRIGGERD("R3"))
		questBoardUIController->SetInputState(QuestBoardUIController::InputState::R3);

	// 前の入力コントローラーに戻す
	if (!questBoardUIController->GetActor()->IsActive())
	{
		_inputManager->SwitchPreviousInput();
	}
}
