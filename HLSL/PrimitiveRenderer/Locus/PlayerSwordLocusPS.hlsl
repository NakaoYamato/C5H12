#include "../PrimitiveRenderer.hlsli"
#include "../../CBuffer/B0/SceneCB.hlsli"

#include "../../Define/SamplerStateDefine.hlsli"
SamplerState samplerStates[_SAMPLER_STATE_MAX] : register(s0);

Texture2D<float4> gbufferColor : register(t2);
Texture2D colorMap : register(t0);
Texture2D parameterMap : register(t1);

cbuffer CbLocus : register(b2)
{
    float distanceRate;
    float colorMinValue;
    float colorMaxValue;
    float locusPadding;
    
    float4 baseColor;
}

float4 main(VS_OUT pin) : SV_TARGET
{
    float4 distance = parameterMap.Sample(samplerStates[_POINT_CLAMP_SAMPLER_INDEX], pin.texcoord);
    float rate = saturate(length(distance.xyz));
    float2 toCenter = abs(pin.texcoord - float2(0.5f, 0.5f));
    float2 vec = toCenter * distanceRate * rate;
    float2 screenUV = pin.position.xy / viewportSize;
    float4 color = gbufferColor.Sample(samplerStates[_POINT_CLAMP_SAMPLER_INDEX], screenUV + vec);
    
    float cValue = length(toCenter * 2.0f);
    if (cValue > colorMaxValue)
    {
        discard;
    }
    else if (cValue > colorMinValue)
    {
        color = color * (1.0f - cValue) + baseColor * cValue;
        color.a = (1.0f - cValue);

    }
    
    return color;
}
