#include "MenuInput.h"

#include <imgui.h>

// 更新処理
void MenuInput::OnUpdate(float elapsedTime)
{
	// 入力状態を更新
	_inputFlags = 0;
	if (_INPUT_TRIGGERD("Up"))
		_inputFlags |= static_cast<int>(InputType::Up);
	if (_INPUT_TRIGGERD("Down"))
		_inputFlags |= static_cast<int>(InputType::Down);
	if (_INPUT_TRIGGERD("Left"))
		_inputFlags |= static_cast<int>(InputType::Left);
	if (_INPUT_TRIGGERD("Right"))
		_inputFlags |= static_cast<int>(InputType::Right);
	if (_INPUT_TRIGGERD("Select"))
		_inputFlags |= static_cast<int>(InputType::Select);
	if (_INPUT_TRIGGERD("Back"))
		_inputFlags |= static_cast<int>(InputType::Back);
}
// GUI描画
void MenuInput::DrawGui()
{
	bool flag = false;
	flag = IsInput(InputType::Up);
	ImGui::Checkbox("Up", &flag);
	flag = IsInput(InputType::Down);
	ImGui::Checkbox("Down", &flag);
	flag = IsInput(InputType::Left);
	ImGui::Checkbox("Left", &flag);
	flag = IsInput(InputType::Right);
	ImGui::Checkbox("Right", &flag);
	flag = IsInput(InputType::Select);
	ImGui::Checkbox("Select", &flag);
	flag = IsInput(InputType::Back);
	ImGui::Checkbox("Back", &flag);
}

void MenuInput::CloseMenu()
{
	// 直前の入力コントローラーに戻す
	_inputManager->SwitchPreviousInput();
}
