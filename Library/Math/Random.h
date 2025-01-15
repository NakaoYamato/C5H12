#pragma once

#include <random>

namespace Random
{
	inline float Rand() { return static_cast<float>(std::rand()); }
	inline float RandNormal() { return Random::Rand() / static_cast<float>(RAND_MAX); }
	inline float Rand(float min, float max){
		return Random::RandNormal() * (max - min) + min;
	}
	inline float RandOrigin() { return Random::Rand(-1.0f, 1.0f); }
	inline float RandAngle() { return 6.28318530718f * RandNormal(); }
}