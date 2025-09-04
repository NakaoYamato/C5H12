#include "CanvasMediator.h"

#include "../../Library/Graphics/Graphics.h"

#include <imgui.h>

// 生成時処理
void CanvasMediator::OnCreate()
{
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
		_userHealthUI->DrawUI(rc, Vector2::Zero, Vector2::One);
	}
	float index = 1.0f;
	for (auto& controller : _otherUserHealthUI)
	{
		controller->DrawUI(rc, _healthUIInterval * index, _healthUIScale);
		index += 1.0f;
	}
}
// GUI描画
void CanvasMediator::OnDrawGui()
{
	ImGui::DragFloat2(u8"HPUI間隔", &_healthUIInterval.x, 0.1f, -1000.0f, 1000.0f);
	ImGui::DragFloat2(u8"HPUIスケール", &_healthUIScale.x, 0.01f, 0.01f, 10.0f);
}
