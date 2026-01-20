#pragma once

#include "../../Library/Math/Vector.h"

class Fade
{
public:
	enum class Type
	{
		FadeIn,
		FadeOut,
		None,
	};

public:
	Fade() {}
	~Fade() {}
	// フェード開始
	void Start(Type type, float duration)
	{
		_duration = duration;
		_elapsedTime = 0.0f;
		_type = type;
		_isActive = true;
	}
	// 更新処理
	void Update(float elapsedTime)
	{
		if (!_isActive)
			return;

		// ローディング等で時間が飛ぶ場合があるので
		// 0.5秒以上の経過時間は無視する
		if (elapsedTime > 0.5f)
			return;

		if (_elapsedTime < _duration)
		{
			_elapsedTime += elapsedTime;
			if (_elapsedTime > _duration)
			{
				_elapsedTime = _duration;
				_isActive = false;
			}
		}
	}
	// フェード中か
	bool IsFading() const
	{
		return _isActive;
	}
	// アルファ値取得
	float GetAlpha() const
	{
		if (_duration <= 0.0f)
			return 0.0f;

		float t = _elapsedTime / _duration;
		if (_type == Type::FadeIn)
			return 1.0f - t;
		else
			return t;
	}
	Type GetType() const
	{
		return _type;
    }
private:
	float _duration = 1.0f;
	float _elapsedTime = 0.0f;
	Type _type = Type::None;
	bool _isActive = false;
};