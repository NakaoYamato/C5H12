#include "ChestInput.h"

#include <imgui.h>

// GUI描画
void ChestInput::DrawGui()
{
}

// 更新時処理
void ChestInput::OnUpdate(float elapsedTime)
{
	// 前の入力コントローラーに戻す
	if (_INPUT_TRIGGERD("Menu"))
	{
		_inputManager->SwitchPreviousInput();
	}
}
