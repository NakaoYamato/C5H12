#include "../../Sprite/Sprite.hlsli"

Texture2D texture0 : register(t0);
#include "../../Define/SamplerStateDefine.hlsli"
SamplerState samplerStates[_SAMPLER_STATE_MAX] : register(s0);

cbuffer CONSTANT_BUFFER : register(b1)
{
    float2 brushUVPosition;
    float brushRadius;
    float brushStrength;

    float4 brushColor;
}

float4 main(VsOut pin) : SV_TARGET
{
    // ƒuƒ‰ƒV‚Ì‰e‹¿“x‚ğó‚¯‚é‚©”»’è
    float len = length(pin.texcoord - brushUVPosition);
    clip(brushRadius - len);

    // ‰e‹¿Š„‡‚ğŒvZ
    float rate = 1.0f - saturate(len / brushRadius);

    float3 color = brushColor.rgb;
    float4 baseColor = texture0.Sample(samplerStates[_POINT_WRAP_SAMPLER_INDEX], pin.texcoord);
    color = lerp(color, baseColor.rgb, brushStrength * rate);
    
    return float4(color, baseColor.a);
}