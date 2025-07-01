#include "../../Sprite/Sprite.hlsli"

Texture2D<float4> texture0 : register(t0);
#include "../../Define/SamplerStateDefine.hlsli"
SamplerState samplerStates[_SAMPLER_STATE_MAX] : register(s0);

cbuffer CONSTANT_BUFFER : register(b1)
{
    float2 brushUVPosition;
    float brushRadius;
    float brushStrength;

    float4 brushColor;
    
    float2 heightScale; // x : ç≈è¨ílÅA y : ç≈ëÂíl
    float2 brushPadding;
}