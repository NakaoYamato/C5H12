#include "../../Sprite/Sprite.hlsli"

Texture2D<float4> colorTexture : register(t0);
Texture2D<float4> normalTexture : register(t1);
Texture2D<float4> parameterTexture : register(t2);

Texture2D<float4> brushColorTexture : register(t3);
Texture2D<float4> brushNormalTexture : register(t4);
#include "../../Define/SamplerStateDefine.hlsli"
SamplerState samplerStates[_SAMPLER_STATE_MAX] : register(s0);

cbuffer CONSTANT_BUFFER : register(b1)
{
    float2 brushUVPosition;
    float brushRadius;
    float brushStrength;
    
    float  textureTillingScale;
    float2 heightScale; // x : ç≈è¨ílÅA y : ç≈ëÂíl
    float1 brushPadding;
}

struct PS_OUT
{
    float4 color : SV_TARGET0;
    float4 normal : SV_TARGET1;
};
