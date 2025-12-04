#include "MenuInput.h"

#include "../../Library/Graphics/Graphics.h"
#include "../../Library/Scene/Scene.h"
#include "../../Library/Scene/SceneManager.h"
#include "../../Source/InGame/InGameCanvasActor.h"

#include <imgui.h>

// 更新処理
void MenuInput::OnUpdate(float elapsedTime)
{
    auto menuUIController = _menuUIController.lock();
	if (!menuUIController)
        return;

	unsigned int currentInputDirection = 0;

	if (_INPUT_PRESSED("Up"))
		currentInputDirection |= static_cast<unsigned int>(MenuUIController::InputDirection::Up);
	if (_INPUT_PRESSED("Down"))
		currentInputDirection |= static_cast<unsigned int>(MenuUIController::InputDirection::Down);

	// 連続入力処理
	if (currentInputDirection != 0 &&
		currentInputDirection & _previousInputDirection)
	{
		if (_inputHoldTime >= _inputHoldThreshold)
		{
			_inputHoldTime -= _inputRepeatInterval;
			menuUIController->SetInputDirection(currentInputDirection);
		}
		else
		{
			menuUIController->SetInputDirection(0);
		}

		_inputHoldTime += elapsedTime;
	}
	else
	{
		_inputHoldTime = 0.0f;
		menuUIController->SetInputDirection(currentInputDirection);
	}
	_previousInputDirection = currentInputDirection;

	if (_INPUT_TRIGGERD("Select"))
		menuUIController->SetInputState(MenuUIController::InputState::Select);
	if (_INPUT_TRIGGERD("Back"))
		menuUIController->SetInputState(MenuUIController::InputState::Back);
	if (_INPUT_TRIGGERD("L3"))
		menuUIController->SetInputState(MenuUIController::InputState::L3);
	if (_INPUT_TRIGGERD("R3"))
		menuUIController->SetInputState(MenuUIController::InputState::R3);

	// 前の入力コントローラーに戻す
	if (!menuUIController->GetActor()->IsActive())
	{
		_inputManager->SwitchPreviousInput();
	}
}
// 開始処理
void MenuInput::Start()
{
	// キャンバスからチェストUIを取得
	if (auto canvas = GetActor()->GetScene()->GetActorManager().FindByClass<InGameCanvasActor>(ActorTag::UI))
	{
		for (auto& child : canvas->GetChildren())
		{
			auto menuUIController = child->GetComponent<MenuUIController>();
			if (menuUIController)
			{
				_menuUIController = menuUIController;
				_menuUIController.lock()->Close();
				break;
			}
		}
	}
}
// GUI描画
void MenuInput::DrawGui()
{
	ImGui::Text(u8"入力の継続時間 :%f", _inputHoldTime);
	ImGui::DragFloat(u8"連続入力までの時間", &_inputHoldThreshold, 0.01f, 0.0f, 1.0f);
	ImGui::DragFloat(u8"連続入力時の入力間隔時間", &_inputRepeatInterval, 0.01f, 0.0f, 1.0f);
}

void MenuInput::CloseMenu()
{
	// 直前の入力コントローラーに戻す
	_inputManager->SwitchPreviousInput();
}

// 起動時関数
void MenuInput::OnEntry()
{
    _menuUIController.lock()->Open();
}

// 終了時処理
void MenuInput::OnExit()
{
    _menuUIController.lock()->Close();
}
