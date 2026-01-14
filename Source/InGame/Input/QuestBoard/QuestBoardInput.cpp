#include "QuestBoardInput.h"

#include "../../Library/Scene/Scene.h"
#include "../../Source/InGame/InGameCanvasActor.h"

#include <imgui.h>

// 開始処理
void QuestBoardInput::Start()
{
	// カメラは動かせない
	SetCanMoveCamera(false);

	// UIを取得
	if (auto questBoardUIActor = GetActor()->GetScene()->GetActorManager().FindByClass<QuestBoardUIActor>(ActorTag::UI))
	{
		_questBoardUIActor = questBoardUIActor;
		_questBoardUIActor.lock()->SetIsActive(false);
	}
}

// GUI描画
void QuestBoardInput::DrawGui()
{
}

// 起動時関数
void QuestBoardInput::OnEntry()
{
	_questBoardUIActor.lock()->SetIsActive(true);
	_questBoardUIActor.lock()->PushPage("SelectMenu");
}

// 終了時関数
void QuestBoardInput::OnExit()
{
	_questBoardUIActor.lock()->SetIsActive(false);
}

// 更新時処理
void QuestBoardInput::OnUpdate(float elapsedTime)
{
	auto questBoardUIActor = _questBoardUIActor.lock();
	if (!questBoardUIActor)
		return;
	auto currentWidget = questBoardUIActor->GetCurrentWidget();
	if (!currentWidget)
	{
		// 前の入力コントローラーに戻す
		_inputManager->SwitchPreviousInput();
	}

	if (_INPUT_REPEAT("Up"))
		currentWidget->SubSelectedOptionIndex();
	if (_INPUT_REPEAT("Down"))
		currentWidget->AddSelectedOptionIndex();

	if (_INPUT_TRIGGERD("Select"))
		currentWidget->SelectOption(questBoardUIActor.get());
	if (_INPUT_TRIGGERD("Back") || _INPUT_TRIGGERD("Menu"))
		currentWidget->BackOption(questBoardUIActor.get());
}
