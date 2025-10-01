#include "../PrimitiveRenderer.hlsli"

#include "../../Define/SamplerStateDefine.hlsli"
SamplerState samplerStates[_SAMPLER_STATE_MAX] : register(s0);

Texture2D<float4> gbufferColor : register(t2);
Texture2D colorMap : register(t3);
Texture2D parameterMap : register(t4);

float4 main(VS_OUT pin) : SV_TARGET
{
    float4 distance = parameterMap.Sample(samplerStates[_POINT_WRAP_SAMPLER_INDEX], pin.texcoord);
    float rate = saturate(length(distance.xyz));
    if (rate > 0.5f && rate < 0.7f)
        return float4(1.0f, 1.0f, 1.0f, 1.0f) * pin.texcoord.y;
        //return float4(rate, 0.0f, 0.0f, 1.0f);
    float2 vec = (pin.texcoord - float2(0.5f, 0.5f)) * 0.3f * rate;
    float2 screenUV = pin.position.xy / viewportSize;
        //float2 uv = screenUV + (distance.xy * 2.0f - float2(1.0f, 1.0f));
    float4 color = gbufferColor.Sample(samplerStates[_POINT_WRAP_SAMPLER_INDEX], screenUV + vec);
    return color;
}
