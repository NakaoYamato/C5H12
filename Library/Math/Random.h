#pragma once

#include <random>

namespace Random
{
	/// <summary>
    /// 浮動小数点数の乱数を生成
	/// </summary>
	/// <returns></returns>
	inline float Rand() { return static_cast<float>(std::rand()); }
	/// <summary>
    /// 0.0fから1.0fの範囲の乱数を生成
	/// </summary>
	/// <returns></returns>
	inline float Rand01() { return Random::Rand() / static_cast<float>(RAND_MAX); }
	/// <summary>
	/// 浮動小数点数の乱数を生成
	/// </summary>
	/// <param name="min">最小値</param>
	/// <param name="max">最大値</param>
	/// <returns></returns>
	inline float Rand(float min, float max)
	{
		return Random::Rand01() * (max - min) + min;
	}
	/// <summary>
    /// -1.0fから1.0fの範囲の乱数を生成
	/// </summary>
	/// <returns></returns>
	inline float RandBias() { return Random::Rand(-1.0f, 1.0f); }
	/// <summary>
    /// ランダムなラジアンを生成
	/// </summary>
	/// <returns></returns>
	inline float RandAngle() { return 6.28318530718f * Rand01(); }
}