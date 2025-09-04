#include "../../CBuffer/B0/SceneCB.hlsli"
#include "../../Sprite/Sprite.hlsli"

Texture2D<float4> colorTexture : register(t0);
Texture2D<float4> normalTexture : register(t1);
Texture2D<float4> parameterTexture : register(t2);

Texture2D<float4> paintColorTexture : register(t4);
Texture2D<float4> paintNormalTexture : register(t5);
Texture2D<float4> brushTexture : register(t7);
#include "../../Define/SamplerStateDefine.hlsli"
SamplerState samplerStates[_SAMPLER_STATE_MAX] : register(s0);

cbuffer CONSTANT_BUFFER : register(b1)
{
    float2 brushUVPosition;
    float brushRadius;
    float brushStrength;
    
    float textureTillingScale;
    float brushRotationY; // Y軸回転角度（ラジアン）
    float2 padding;
}

struct PS_OUT
{
    float4 color : SV_TARGET0;
    float4 normal : SV_TARGET1;
};

// ブレンドレートの計算
float CalculateBrushRate(float2 texcoord)
{
    // 影響割合を計算
    float2 vec = texcoord - brushUVPosition;
    // ブラシの回転を適用してブラシのUV座標を計算
    float2 brushTexcoord =
        float2(0.5f, 0.5f) +
        float2(vec.x * cos(brushRotationY) - vec.y * sin(brushRotationY),
               vec.x * sin(brushRotationY) + vec.y * cos(brushRotationY)) / brushRadius / 2.0f;
    return saturate(brushTexture.Sample(samplerStates[_BORDER_POINT_SAMPLER_INDEX], brushTexcoord).r);
}
