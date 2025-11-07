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
				_chestUIController.lock()->GetActor()->SetIsActive(false);
				break;
			}
		}
	}
}

// GUI描画
void ChestInput::DrawGui()
{
}

// 起動時関数
void ChestInput::OnExecute()
{
	_chestUIController.lock()->GetActor()->SetIsActive(true);
}

// 終了時関数
void ChestInput::OnEnd()
{
	_chestUIController.lock()->GetActor()->SetIsActive(false);
}

// 更新時処理
void ChestInput::OnUpdate(float elapsedTime)
{
	auto chestUIController = _chestUIController.lock();
	if (!chestUIController)
		return;

	if (_INPUT_TRIGGERD("Up"))
		chestUIController->AddIndex(-1);
	if (_INPUT_TRIGGERD("Down"))
		chestUIController->AddIndex(+1);

	// 前の入力コントローラーに戻す
	if (_INPUT_TRIGGERD("Menu"))
	{
		_inputManager->SwitchPreviousInput();
	}
}
