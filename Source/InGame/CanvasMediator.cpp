#include "CanvasMediator.h"

#include "../../Library/Graphics/Graphics.h"

#include "../../Source/InGame/UI/TimerUIController.h"

#include <imgui.h>

// 生成時処理
void CanvasMediator::OnCreate()
{

	// タイマー生成
    auto timerUIActor = GetScene()->RegisterActor<UIActor>("TimerUI", ActorTag::UI);
    timerUIActor->SetParent(this);
    auto timerUIController = timerUIActor->AddComponent<TimerUIController>();
}
// 開始時処理
void CanvasMediator::OnStart()
{
}
// 更新前処理
void CanvasMediator::OnPreUpdate(float elapsedTime)
{
}
// 遅延更新処理
void CanvasMediator::OnLateUpdate(float elapsedTime)
{
}
// 固定間隔更新処理
void CanvasMediator::OnFixedUpdate()
{
}
// UI描画処理
void CanvasMediator::OnDelayedRender(const RenderContext& rc)
{
	// HPUIの描画
	if (_userHealthUI)
	{
		_userHealthUI->DrawUI(rc);
	}
	float index = 1.0f;
	for (auto& controller : _otherUserHealthUI)
	{
		controller->DrawUI(rc);
		index += 1.0f;
	}
}
// GUI描画
void CanvasMediator::OnDrawGui()
{
}
