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

	for (auto& [name, spriteData] : _spriteDatas)
	{
		if (ImGui::TreeNode(name.c_str()))
		{
			if (ImGui::Combo(u8"中心位置", reinterpret_cast<int*>(&spriteData.centerAlignment), alignmentNames, IM_ARRAYSIZE(alignmentNames)))
				spriteData.RecalcCenter(spriteData.centerAlignment);

			ImGui::DragFloat2(u8"位置",			&spriteData.position.x, 0.1f, -1000.0f, 1000.0f);
			ImGui::DragFloat2(u8"スケール",		&spriteData.scale.x, 0.01f, 0.01f, 1000.0f);
			ImGui::DragFloat2(u8"画像位置",		&spriteData.texPos.x, 0.01f, 0.01f, 1000.0f);
			ImGui::DragFloat2(u8"画像サイズ",	&spriteData.texSize.x, 0.01f, 0.01f, 1000.0f);
			ImGui::DragFloat2(u8"中心",			&spriteData.center.x, 0.01f, -1000.0f, 1000.0f);
			ImGui::DragFloat(u8"角度",			&spriteData.angle, 0.1f, -360.0f, 360.0f);
			ImGui::ColorEdit4(u8"色",			&spriteData.color.x);

			ImGui::TreePop();
		}
	}
}

// 画像読み込み
void UIController::LoadTexture(const std::string& spriteName, const wchar_t* filename, CenterAlignment alignment)
{
	_spriteDatas[spriteName].sprite = std::make_unique<Sprite>(Graphics::Instance().GetDevice(), filename);
	_spriteDatas[spriteName].RecalcCenter(alignment);
}

void UIController::SpriteRender(
	const std::string& spriteName,
	const RenderContext& rc, 
	const Vector2& offset,
	const Vector2& offsetScale)
{
	if (_spriteDatas[spriteName].sprite)
	{
		_spriteDatas[spriteName].sprite->Render(rc.deviceContext,
			_spriteDatas[spriteName].position + offset,
			{ _spriteDatas[spriteName].scale.x * offsetScale.x, _spriteDatas[spriteName].scale.y * offsetScale.y },
			_spriteDatas[spriteName].texPos,
			_spriteDatas[spriteName].texSize,
			_spriteDatas[spriteName].center,
			DirectX::XMConvertToRadians(_spriteDatas[spriteName].angle),
			_spriteDatas[spriteName].color);
	}
}

// 中心位置を再計算
void UIController::SpriteData::RecalcCenter(CenterAlignment alignment)
{
	switch (alignment)
	{
	case LEFT_UP:
		center = { 0.0f,0.0f };
		break;
	case LEFT_CENTER:
		center = { 0.0f,texSize.y / 2.0f };
		break;
	case LEFT_DOWN:
		center = { 0.0f,texSize.y };
		break;
	case CENTER_UP:
		center = { texSize.x / 2.0f,0.0f };
		break;
	case CENTER_CENTER:
		center = { texSize.x / 2.0f,texSize.y / 2.0f };
		break;
	case CENTER_DOWN:
		center = { texSize.x / 2.0f,texSize.y };
		break;
	case RIGHT_UP:
		center = { texSize.x,0.0f };
		break;
	case RIGHT_CENTER:
		center = { texSize.x,texSize.y / 2.0f };
		break;
	case RIGHT_DOWN:
		center = { texSize.x,texSize.y };
		break;
	default:
		center = { 0.0f,0.0f };
		break;
	}
}
