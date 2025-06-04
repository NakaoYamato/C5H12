#include "../ComputeParticle.hlsli"
#include "../../Define/SamplerStateDefine.hlsli"
SamplerState samplerStates[_SAMPLER_STATE_MAX] : register(s0);

Texture2D colorMap : register(t0);

float4 main(PS_IN pin) : SV_TARGET0
{
    float4 color = colorMap.Sample(samplerStates[_ANISOTROPIC_SAMPLER_INDEX], pin.texcoord) * pin.color;
    clip(color.a < 0.1f ? -1 : 1);
    return color;
}