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
			//auto chestUIController = child->GetComponent<ChestUIController>();
			//if (chestUIController)
			//{
			//	_chestUIController = chestUIController;
			//	_chestUIController.lock()->Close();
			//	break;
			//}
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
}

// 終了時関数
void QuestBoardInput::OnExit()
{
}

// 更新時処理
void QuestBoardInput::OnUpdate(float elapsedTime)
{


	// 前の入力コントローラーに戻す
	if (_INPUT_TRIGGERD("Back") || _INPUT_TRIGGERD("Menu"))
	{
		_inputManager->SwitchPreviousInput();
	}
}
