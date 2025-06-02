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

// GUI描画
void UIController::DrawGui()
{
	static const char* alignmentNames[] = {	
		"LEFT_UP",
		"LEFT_CENTER",
		"LEFT_DOWN",
		"CENTER_UP",
		"CENTER_CENTER",
		"CENTER_DOWN",
		"RIGHT_UP",
		"RIGHT_CENTER",
		"RIGHT_DOWN"
	};
	if (ImGui::Combo(u8"中心位置", reinterpret_cast<int*>(&_centerAlignment), alignmentNames, IM_ARRAYSIZE(alignmentNames)))
		RecalcCenter(_centerAlignment);

	ImGui::DragFloat2(u8"位置",		&_position.x, 0.1f, -1000.0f, 1000.0f);
	ImGui::DragFloat2(u8"スケール",	&_scale.x, 0.01f, 0.01f, 1000.0f);
	ImGui::DragFloat2(u8"画像位置",	&_texPos.x, 0.01f, 0.01f, 1000.0f);
	ImGui::DragFloat2(u8"画像サイズ",	&_texSize.x, 0.01f, 0.01f, 1000.0f);
	ImGui::DragFloat2(u8"中心",		&_center.x, 0.01f, -1000.0f, 1000.0f);
	ImGui::DragFloat(u8"角度",		&_angle, 0.1f, -360.0f, 360.0f);
	ImGui::ColorEdit4(u8"色",		&_color.x);
}

// 画像読み込み
void UIController::LoadTexture(const wchar_t* filename, CenterAlignment alignment)
{
	_sprite = std::make_unique<Sprite>(Graphics::Instance().GetDevice(), filename);
	RecalcCenter(alignment);
}

// 中心位置を再計算
void UIController::RecalcCenter(CenterAlignment alignment)
{
	switch (alignment)
	{
	case LEFT_UP:
		_center = { 0.0f,0.0f };
		break;
	case LEFT_CENTER:
		_center = { 0.0f,_texSize.y / 2.0f };
		break;
	case LEFT_DOWN:
		_center = { 0.0f,_texSize.y };
		break;
	case CENTER_UP:
		_center = { _texSize.x / 2.0f,0.0f };
		break;
	case CENTER_CENTER:
		_center = { _texSize.x / 2.0f,_texSize.y / 2.0f };
		break;
	case CENTER_DOWN:
		_center = { _texSize.x / 2.0f,_texSize.y };
		break;
	case RIGHT_UP:
		_center = { _texSize.x,0.0f };
		break;
	case RIGHT_CENTER:
		_center = { _texSize.x,_texSize.y / 2.0f };
		break;
	case RIGHT_DOWN:
		_center = { _texSize.x,_texSize.y };
		break;
	default:
		_center = { 0.0f,0.0f };
		break;
	}
}

void UIController::SpriteRender(
	const RenderContext& rc, 
	const Vector2& offset,
	const Vector2& offsetScale)
{
	if (_sprite)
	{
		_sprite->Render(rc.deviceContext,
			_position + offset,
			{ _scale.x * offsetScale.x, _scale.y * offsetScale.y },
			_texPos,
			_texSize,
			_center,
			DirectX::XMConvertToRadians(_angle),
			_color);
	}
}
