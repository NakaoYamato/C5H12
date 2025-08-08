#include "PrimitiveRenderer.hlsli"

#include "../Define/SamplerStateDefine.hlsli"
SamplerState samplerStates[_SAMPLER_STATE_MAX] : register(s0);

Texture2D<float4> gbufferColor : register(t2);
Texture2D distanceMap : register(t3);
Texture2D noiseMap : register(t4);

float4 main(VS_OUT pin) : SV_TARGET
{
    float2 noiseTexcoord = pin.texcoord;
    noiseTexcoord.x *= 0.0025f;
    float noise = noiseMap.Sample(samplerStates[_POINT_WRAP_SAMPLER_INDEX], noiseTexcoord).r;
    
    float2 distanceTexcoord = noiseTexcoord;
    distanceTexcoord.x -= totalElapsedTime * 0.05f;
    float4 distance = distanceMap.Sample(samplerStates[_POINT_WRAP_SAMPLER_INDEX], distanceTexcoord);
    
    float2 screenUV = pin.position.xy / viewportSize;
    float2 uv = screenUV + (distance.xy * 2.0f - float2(1.0f, 1.0f) * 0.01f);
    float4 color = gbufferColor.Sample(samplerStates[_POINT_WRAP_SAMPLER_INDEX], frac(uv));
    //return color;
    
    return float4(pin.color.rgb * noise + color.rgb * (1.0f - noise), 1.0f);
}
