#include "../../Sprite/Sprite.hlsli"

cbuffer BLOOM_CONSTANT_BUFFER : register(b1)
{
    float bloom_extraction_threshold;
    float bloom_intensity;
    float2 something;
};