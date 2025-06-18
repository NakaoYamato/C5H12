#include "MenuInput.h"

#include <imgui.h>

// 開始処理
void MenuInput::Start()
{
	FindInputMediator();
}
// 更新処理
void MenuInput::Update(float elapsedTime)
{
	// 起動状態でなければ処理しない
	if (!IsActive())
		return;

	// 入力状態を更新
	_inputFlags = 0;
	if (_INPUT_IS_TRIGGERD("Up"))
		_inputFlags |= static_cast<int>(InputType::Up);
	if (_INPUT_IS_TRIGGERD("Down"))
		_inputFlags |= static_cast<int>(InputType::Down);
	if (_INPUT_IS_TRIGGERD("Left"))
		_inputFlags |= static_cast<int>(InputType::Left);
	if (_INPUT_IS_TRIGGERD("Right"))
		_inputFlags |= static_cast<int>(InputType::Right);
	if (_INPUT_IS_TRIGGERD("Select"))
		_inputFlags |= static_cast<int>(InputType::Select);
	if (_INPUT_IS_TRIGGERD("Back"))
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
// 他のInputControllerから命令を受信
void MenuInput::ReceiveCommandFromOther(InputMediator::CommandType commandType, const std::string& command)
{
	switch (commandType)
	{
	case InputMediator::CommandType::StartGame:
		// ゲーム開始の命令を受け取った場合、非アクティブにする
		SetActive(false);
		break;
	case InputMediator::CommandType::OpenMenu:
		// メニュー画面を開く命令を受け取った場合、アクティブにする
		SetActive(true);
		break;
	case InputMediator::CommandType::CloseMenu:
		// メニュー画面を閉じる命令を受け取った場合、非アクティブにする
		SetActive(false);
		break;
	default:
		break;
	}
}

void MenuInput::CloseMenu()
{
	if (_inputMediator.lock())
	{
		_inputMediator.lock()->ReceiveCommand(this, InputMediator::CommandType::CloseMenu, "");
	}
}
