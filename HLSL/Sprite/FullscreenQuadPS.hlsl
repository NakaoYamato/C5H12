#include "Sprite.hlsli"

Texture2D colorMap : register(t0);
#include "../Define/SamplerStateDefine.hlsli"
SamplerState samplerStates[_SAMPLER_STATE_MAX] : register(s0);

float4 main(VsOut pin) : SV_TARGET
{
    float4 color = colorMap.Sample(samplerStates[_POINT_WRAP_SAMPLER_INDEX], pin.texcoord) * pin.color;

    return color;
}