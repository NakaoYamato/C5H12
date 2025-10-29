#include "CanvasMediator.h"

#include "../../Library/Graphics/Graphics.h"

#include <imgui.h>

// ¶¬Žžˆ—
void CanvasMediator::OnCreate()
{
}
// ŠJŽnŽžˆ—
void CanvasMediator::OnStart()
{
}
// XV‘Oˆ—
void CanvasMediator::OnPreUpdate(float elapsedTime)
{
}
// ’x‰„XVˆ—
void CanvasMediator::OnLateUpdate(float elapsedTime)
{
}
// ŒÅ’èŠÔŠuXVˆ—
void CanvasMediator::OnFixedUpdate()
{
}
// UI•`‰æˆ—
void CanvasMediator::OnDelayedRender(const RenderContext& rc)
{
	// HPUI‚Ì•`‰æ
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
// GUI•`‰æ
void CanvasMediator::OnDrawGui()
{
}
