#pragma once

#include "../../Library/2D/SpriteResource.h"
#include "../../Library/Math/RectTransform.h"
#include "../../Library/Math/Easing.h"
#include "../../Library/Graphics/RenderContext.h"

#include <string>
#include <memory>

class Sprite
{
public:
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
	virtual void Render(const RenderContext& rc);
	// GUI描画
	void DrawGui();
	// 画像読み込み
	void LoadTexture(const wchar_t* filename, CenterAlignment alignment);
	// 画像との当たり判定
	bool IsHit(const Vector2& pos) const;
	// 中心位置を再計算
	void RecalcCenter(CenterAlignment alignment);

#pragma region アクセサ
	RectTransform& GetRectTransform() { return _rectTransform; }

	CenterAlignment GetCenterAlignment()const { return _centerAlignment; }
	const Vector2& GetTexPos()const { return _texPos; }
	const Vector2& GetTexSize()const { return _texSize; }
	const Vector2& GetCenter()const { return _center; }
	const Vector4& GetColor()const { return _color; }

	void SetCenterAlignment(CenterAlignment alignment) {
		_centerAlignment = alignment;
		RecalcCenter(alignment);
	}
	void SetTexPos(const Vector2& p) { _texPos = p; }
	void SetTexSize(const Vector2& s) { _texSize = s; }
	void SetCenter(const Vector2& c) { _center = c; }
	void SetColor(const Vector4& c) { _color = c; }
#pragma endregion
private:
	std::unique_ptr<SpriteResource> _sprite;
	RectTransform _rectTransform;

	CenterAlignment	_centerAlignment = CenterAlignment::CenterCenter;
	Vector2			_texPos = Vector2::Zero;
	Vector2			_texSize = Vector2::Zero;
	Vector2			_center = Vector2::Zero;
	Vector4			_color = Vector4::White;
};