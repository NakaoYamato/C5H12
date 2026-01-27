#include "OptionInput.h"

#include "../../Library/Scene/Scene.h"
#include "../../Source/InGame/InGameCanvasActor.h"

// 開始処理
void OptionInput::Start()
{
	// カメラは動かせない
	SetCanMoveCamera(false);

	// UIを取得
	if (auto optionUIActor = GetActor()->GetScene()->GetActorManager().FindByClass<OptionUIActor>(ActorTag::UI))
	{
		_optionUIActor = optionUIActor;
		_optionUIActor.lock()->SetIsActive(false);
	}
}

// GUI描画
void OptionInput::DrawGui()
{
}

// 起動時関数
void OptionInput::OnEntry()
{
	_optionUIActor.lock()->SetIsActive(true);
}

// 終了時関数
void OptionInput::OnExit()
{
	_optionUIActor.lock()->SetIsActive(false);
}

// 更新時処理
void OptionInput::OnUpdate(float elapsedTime)
{
	auto optionUIActor = _optionUIActor.lock();
	if (!optionUIActor)
		return;
	auto currentWidget = optionUIActor->GetCurrentWidget();
	if (!currentWidget)
	{
		// 前の入力コントローラーに戻す
		_inputManager->SwitchPreviousInput();
		return;
	}
}
