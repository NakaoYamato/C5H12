#include "PrimitiveRenderer.hlsli"

#include "../Define/SamplerStateDefine.hlsli"
SamplerState samplerStates[_SAMPLER_STATE_MAX] : register(s0);

Texture2D<float4> gbufferColor : register(t2);

float4 main(VS_OUT pin) : SV_TARGET
{
    //return pin.color;
    
    float2 uv = float2(0.0f, 0.0f);
    uv.x = (pin.position.x / pin.position.w + 1.0f) * 0.5f;
    //uv.y = (1.0f - (pin.position.y / pin.position.w + 1.0f) * 0.5f);
    uv.y = (pin.position.y / pin.position.w + 1.0f) * 0.5f;

    uv = float2(0.5f, 0.5f) + normalize(pin.position.xy) / 2.0f;
    
    //uv.x += 0.001f;
    //uv.y += 0.001f;
    
    //float4 temp = gbufferColor.Sample(samplerStates[_POINT_WRAP_SAMPLER_INDEX], uv);
    //uv.x += temp.x * 0.5f;
    //uv.y += temp.y * 0.5f;
    
    float4 color = gbufferColor.Sample(samplerStates[_POINT_WRAP_SAMPLER_INDEX], uv);
    return color;
    return float4(pin.color.rgb * pin.color.a + color.rgb * (1.0f - pin.color.a), 1.0f);

}
