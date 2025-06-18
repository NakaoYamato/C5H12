#include "Sprite.h"

#include "../../Library/Graphics/Graphics.h"
#include "../../Library/Collision/CollisionMath.h"

#include <imgui.h>

Sprite::Sprite(const wchar_t* filename, CenterAlignment alignment)
{
	LoadTexture(filename, alignment);
}
// GUI描画
void Sprite::DrawGui()
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

	ImGui::DragFloat2(u8"位置", &_position.x, 0.1f, -1000.0f, 1000.0f);
	ImGui::DragFloat2(u8"スケール", &_scale.x, 0.01f, 0.01f, 1000.0f);
	ImGui::DragFloat2(u8"画像位置", &_texPos.x, 0.01f, 0.01f, 1000.0f);
	ImGui::DragFloat2(u8"画像サイズ", &_texSize.x, 0.01f, 0.01f, 1000.0f);
	ImGui::DragFloat2(u8"中心", &_center.x, 0.01f, -1000.0f, 1000.0f);
	ImGui::DragFloat(u8"角度", &_angle, 0.1f, -360.0f, 360.0f);
	ImGui::ColorEdit4(u8"色", &_color.x);
}
// 画像読み込み
void Sprite::LoadTexture(const wchar_t* filename, CenterAlignment alignment)
{
	_sprite = std::make_unique<SpriteResource>(Graphics::Instance().GetDevice(), filename);
	_texSize = _sprite->GetTextureSize();
	RecalcCenter(alignment);
}
// 画像との当たり判定
bool Sprite::IsHit(const Vector2& pos) const
{
	Vector2 aabbHalfSize{};
	{
		aabbHalfSize.x = _texSize.x * _scale.x / 2.0f;
		aabbHalfSize.y = _texSize.y * _scale.y / 2.0f;
	}
	Vector2 aabbCenter{};
	{
		// centerの値から左上の座標を求める
		aabbCenter.x = _position.x - (_center.x / _texSize.x) * aabbHalfSize.x * 2.0f;
		aabbCenter.y = _position.y - (_center.y / _texSize.y) * aabbHalfSize.y * 2.0f;
	}
	aabbCenter += aabbHalfSize;

	return Collision2D::IntersectPointVsAABB(pos, aabbCenter, aabbHalfSize);
}
// 中心位置を再計算
void Sprite::RecalcCenter(CenterAlignment alignment)
{
	switch (alignment)
	{
	case LeftUp:
		_center = { 0.0f,0.0f };
		break;
	case LeftCenter:
		_center = { 0.0f,_texSize.y / 2.0f };
		break;
	case LeftDown:
		_center = { 0.0f,_texSize.y };
		break;
	case CenterUp:
		_center = { _texSize.x / 2.0f,0.0f };
		break;
	case CenterCenter:
		_center = { _texSize.x / 2.0f,_texSize.y / 2.0f };
		break;
	case CenterDown:
		_center = { _texSize.x / 2.0f,_texSize.y };
		break;
	case RightUp:
		_center = { _texSize.x,0.0f };
		break;
	case RightCenter:
		_center = { _texSize.x,_texSize.y / 2.0f };
		break;
	case RightDown:
		_center = { _texSize.x,_texSize.y };
		break;
	default:
		_center = { 0.0f,0.0f };
		break;
	}
}
// スプライト描画
void Sprite::Render(const RenderContext& rc, const Vector2& offset, const Vector2& offsetScale)
{
	if (!_sprite)
		return;

	_sprite->Render(rc.deviceContext,
		_position + offset,
		{ _scale.x * offsetScale.x, _scale.y * offsetScale.y },
		_texPos,
		_texSize,
		_center,
		DirectX::XMConvertToRadians(_angle),
		_color);
}
