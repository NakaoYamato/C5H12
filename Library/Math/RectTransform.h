#pragma once

#include "Vector.h"

class RectTransform
{
public:
	RectTransform() {}
	~RectTransform() {}

	/// <summary>
	/// 更新
	/// </summary>
	/// <param name="parent"></param>
	void UpdateTransform(const RectTransform* parent = nullptr);

	/// <summary>
	/// デバッグGUI表示
	/// </summary>
	void DrawGui();

	/// <summary>
	/// 値をリセット
	/// </summary>
	void Reset();
#pragma region アクセサ
	const Vector2&	GetLocalPosition()const		{ return _localPosition; }
	const Vector2&	GetLocalScale()const		{ return _localScale; }
	float			GetLocalAngle()const		{ return _localAngle; }
	const Vector2&	GetWorldPosition()const		{ return _worldPosition; }
	const Vector2&	GetWorldScale()const		{ return _worldScale; }
	float			GetWorldAngle()const		{ return _worldAngle; }

	void SetLocalPosition(const Vector2& p) { _localPosition = p; }
	void SetLocalScale(const Vector2& s)	{ _localScale = s; }
	void SetLocalAngle(float a)				{ _localAngle = a; }
#pragma endregion
private:
	Vector2			_localPosition		= Vector2::Zero;
	Vector2			_localScale			= Vector2::One;
	float			_localAngle			= 0.0f;

	Vector2 	   _worldPosition		= Vector2::Zero;
	Vector2        _worldScale			= Vector2::One;
	float          _worldAngle			= 0.0f;
};