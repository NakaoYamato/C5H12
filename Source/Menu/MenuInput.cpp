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

	// メニュー画面終了入力
	if (_INPUT_IS_RELEASED("Menu"))
	{
		if (_inputMediator.lock())
		{
			_inputMediator.lock()->ReceiveCommand(this, InputMediator::CommandType::CloseMenu, "");
		}
	}
}
// GUI描画
void MenuInput::DrawGui()
{
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
