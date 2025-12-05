#include "ChestInput.h"

#include "../../Library/Scene/Scene.h"
#include "../../Source/InGame/InGameCanvasActor.h"

#include <imgui.h>

// 開始処理
void ChestInput::Start()
{
	// カメラは動かせない
	SetCanMoveCamera(false);

	// キャンバスからチェストUIを取得
	if (auto canvas = GetActor()->GetScene()->GetActorManager().FindByClass<InGameCanvasActor>(ActorTag::UI))
	{
		for (auto& child : canvas->GetChildren())
		{
			auto chestUIController = child->GetComponent<ChestUIController>();
			if (chestUIController)
			{
				_chestUIController = chestUIController;
				_chestUIController.lock()->Close();
				break;
			}
		}
	}
}

// GUI描画
void ChestInput::DrawGui()
{
	ImGui::Text(u8"入力の継続時間 :%f", _inputHoldTime);
	ImGui::DragFloat(u8"連続入力までの時間", &_inputHoldThreshold, 0.01f, 0.0f, 1.0f);
	ImGui::DragFloat(u8"連続入力時の入力間隔時間", &_inputRepeatInterval, 0.01f, 0.0f, 1.0f);
}

// 起動時関数
void ChestInput::OnEntry()
{
	_chestUIController.lock()->Open();
}

// 終了時関数
void ChestInput::OnExit()
{
	_chestUIController.lock()->Close();
}

// 更新時処理
void ChestInput::OnUpdate(float elapsedTime)
{
	auto chestUIController = _chestUIController.lock();
	if (!chestUIController)
		return;

	unsigned int currentInputDirection = 0;

	if (_INPUT_PRESSED("Up"))
		currentInputDirection |= static_cast<unsigned int>(ChestUIController::InputDirection::Up);
	if (_INPUT_PRESSED("Down"))
		currentInputDirection |= static_cast<unsigned int>(ChestUIController::InputDirection::Down);
	if (_INPUT_PRESSED("Left"))
		currentInputDirection |= static_cast<unsigned int>(ChestUIController::InputDirection::Left);
	if (_INPUT_PRESSED("Right"))
		currentInputDirection |= static_cast<unsigned int>(ChestUIController::InputDirection::Right);

	// 連続入力処理
	if (currentInputDirection != 0 &&
		currentInputDirection & _previousInputDirection)
	{
		if (_inputHoldTime >= _inputHoldThreshold)
		{
			_inputHoldTime -= _inputRepeatInterval;
			chestUIController->SetInputDirection(currentInputDirection);
		}
		else
		{
			chestUIController->SetInputDirection(0);
		}

		_inputHoldTime += elapsedTime;
	}
	else
	{
		_inputHoldTime = 0.0f;
		chestUIController->SetInputDirection(currentInputDirection);
	}

	_previousInputDirection = currentInputDirection;

	if (_INPUT_TRIGGERD("Select"))
		chestUIController->SetInputState(ChestUIController::InputState::Select);
	if (_INPUT_TRIGGERD("Back") || _INPUT_TRIGGERD("Menu"))
		chestUIController->SetInputState(ChestUIController::InputState::Back);
	if (_INPUT_TRIGGERD("L3"))
		chestUIController->SetInputState(ChestUIController::InputState::L3);
	if (_INPUT_TRIGGERD("R3"))
		chestUIController->SetInputState(ChestUIController::InputState::R3);


	// 前の入力コントローラーに戻す
	if (!chestUIController->GetActor()->IsActive())
	{
		_inputManager->SwitchPreviousInput();
	}
}
