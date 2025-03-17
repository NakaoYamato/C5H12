#include "../../Sprite/Sprite.hlsli"

cbuffer BLOOM_CONSTANT_BUFFER : register(b1)
{
    float bloom_intensity;
    float3 something;
};