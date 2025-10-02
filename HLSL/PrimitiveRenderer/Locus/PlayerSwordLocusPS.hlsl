#include "../PrimitiveRenderer.hlsli"

#include "../../Define/SamplerStateDefine.hlsli"
SamplerState samplerStates[_SAMPLER_STATE_MAX] : register(s0);

Texture2D<float4> gbufferColor : register(t2);
Texture2D colorMap : register(t0);
Texture2D parameterMap : register(t1);

float4 main(VS_OUT pin) : SV_TARGET
{
    float4 distance = parameterMap.Sample(samplerStates[_POINT_WRAP_SAMPLER_INDEX], pin.texcoord);
    float rate = saturate(length(distance.xyz));
    float2 vec = (pin.texcoord - float2(0.5f, 0.5f)) * 0.3f * rate;
    float2 screenUV = pin.position.xy / viewportSize;
    float4 color = gbufferColor.Sample(samplerStates[_POINT_WRAP_SAMPLER_INDEX], screenUV + vec);
    return color;
}
