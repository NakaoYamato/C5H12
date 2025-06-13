#pragma once

#include <algorithm>

class MathF
{
public:
	static float Clamp(float value, float min, float max)
	{
		return std::clamp<float>(value, min, max);
	}
	static float Clamp01(float value)
	{
		return value < 0.0f ? 0.0f : (value > 1.0f ? 1.0f : value);
	}
	static float Lerp(float a, float b, float t)
	{
		t = Clamp01(t);
		return a + (b - a) * t;
	}
};