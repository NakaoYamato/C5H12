#include "../../Sprite/Sprite.hlsli"

static const uint DownsampledCount = 6;

cbuffer GAUSSIAN_FILTER : register(b1)
{
    float intensity;
    float3 something;
}