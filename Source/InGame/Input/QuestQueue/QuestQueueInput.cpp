#include "QuestQueueInput.h"

#include "../../Library/Scene/Scene.h"
#include "../../Source/InGame/InGameCanvasActor.h"

#include "../../Source/Quest/QuestController.h"

#include <imgui.h>

// 開始処理
void QuestQueueInput::Start()
{
	// カメラは動かせない
	SetCanMoveCamera(false);

	// UIを取得
	if (auto canvas = GetActor()->GetScene()->GetActorManager().FindByClass<InGameCanvasActor>(ActorTag::UI))
	{
		for (auto& child : canvas->GetChildren())
		{
			auto questQueueUIController = child->GetComponent<QuestQueueUIController>();
			if (questQueueUIController)
			{
				_questQueueUIController = questQueueUIController;
				questQueueUIController->GetActor()->SetIsActive(false);
				break;
			}
		}
	}
}

// GUI描画
void QuestQueueInput::DrawGui()
{
}

// 起動時関数
void QuestQueueInput::OnEntry()
{
	if (auto questQueueUIController = _questQueueUIController.lock())
	{
		questQueueUIController->GetActor()->SetIsActive(true);
	}
}

// 終了時関数
void QuestQueueInput::OnExit()
{
	if (auto questQueueUIController = _questQueueUIController.lock())
	{
		questQueueUIController->GetActor()->SetIsActive(false);
	}
}

// 更新時処理
void QuestQueueInput::OnUpdate(float elapsedTime)
{
	if (_INPUT_TRIGGERD("Select"))
	{
		// クエスト開始
		auto gameManager = GetActor()->GetScene()->GetActorManager().FindByName("GameManager", ActorTag::System);
		if (!gameManager)
			return;
		auto questController = gameManager->GetComponent<QuestController>();
		if (!questController)
			return;
		questController->StartQuest();
	}
	if (_INPUT_TRIGGERD("Back") || _INPUT_TRIGGERD("Menu"))
		_inputManager->SwitchPreviousInput();
}
