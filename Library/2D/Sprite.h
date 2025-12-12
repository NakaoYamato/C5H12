#pragma once

#include "../../Library/Material/Material.h"
#include "../../Library/Math/RectTransform.h"
#include "../../Library/Math/Easing.h"
#include "../../Library/Graphics/RenderContext.h"
#include "../../Library/Renderer/TextureRenderer.h"

#include <string>
#include <memory>

class Sprite
{
public:
	// 中心位置の定義
	enum CenterAlignment
	{
		LeftUp,
		LeftCenter,
		LeftDown,
		CenterUp,
		CenterCenter,
		CenterDown,
		RightUp,
		RightCenter,
		RightDown,
	};

public:
	Sprite() = default;
	Sprite(const wchar_t* filename, CenterAlignment alignment = CenterAlignment::CenterCenter);
	~Sprite() = default;

	// トランスフォーム更新
	virtual void UpdateTransform(RectTransform* parent = nullptr);
	// 描画
	void Render(const RenderContext& rc, TextureRenderer& renderer);
	// 描画
	void Render(const RenderContext& rc, TextureRenderer& renderer, float alpha);
	// 描画
	void Render(const RenderContext& rc,
		TextureRenderer& renderer,
		const Vector2& position,
		const Vector2& scale	= Vector2::One,
		const Vector2& center	= Vector2::Zero,
		float angle				= 0.0f,
		const Vector4& color	= Vector4::White);
	// GUI描画
	void DrawGui();
	// 画像読み込み
	void LoadTexture(const wchar_t* filename, CenterAlignment alignment);
	// 画像との当たり判定
	bool IsHit(const Vector2& pos) const;
	// 中心位置を再計算
	void RecalcCenter(CenterAlignment alignment);

#pragma region アクセサ
	Texture&		GetTexture()		{ return _texture; }
	RectTransform&	GetRectTransform()	{ return _rectTransform; }
	Material&		GetMaterial()		{ return _material; }

	CenterAlignment GetCenterAlignment()const	{ return _centerAlignment; }
	const Vector2&	GetTexPos()const			{ return _texPos; }
	const Vector2&	GetTexSize()const			{ return _texSize; }
	const Vector2&	GetCenter()const			{ return _center; }
	const Vector4&	GetColor()const				{ return _color; }
    DepthState		GetDepthState() const		{ return _depthState; }
	int				GetStencil() const			{ return _stencil; }

	void SetCenterAlignment(CenterAlignment alignment)
	{
		_centerAlignment = alignment;
		RecalcCenter(alignment);
	}
	void SetTexPos(const Vector2& p)			{ _texPos = p; }
	void SetTexSize(const Vector2& s)			{ _texSize = s; }
	void SetCenter(const Vector2& c)			{ _center = c; }
	void SetColor(const Vector4& c)				{ _color = c; }
	void SetColorAlpha(float a) { SetColor(Vector4(GetColor().x, GetColor().y, GetColor().z, a)); }
	void SetDepthState(DepthState depthState)	{ _depthState = depthState; }
    void SetStencil(UINT8 stencil)				{ _stencil = stencil; }
#pragma endregion
private:
	Texture			_texture;
	RectTransform	_rectTransform;
	Material		_material;

	CenterAlignment	_centerAlignment = CenterAlignment::CenterCenter;
	Vector2			_texPos = Vector2::Zero;
	Vector2			_texSize = Vector2::Zero;
	Vector2			_center = Vector2::Zero;
	Vector4			_color = Vector4::White;

    DepthState		_depthState = DepthState::TestAndWrite;
	int				_stencil = 0;
};