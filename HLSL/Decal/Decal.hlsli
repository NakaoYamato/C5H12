#include "../Scene/SceneConstantBuffer.hlsli"

struct PS_OUT
{
    float4 baseColor : SV_TARGET0;
    float4 worldNormal : SV_TARGET1;
};

cbuffer GBUFFER_DECAL_CONSTANT_BUFFER : register(b2)
{
    row_major float4x4 world;
    row_major float4x4 invWorld;
    float4 materialColor;
    float4 decalDirection;
};
