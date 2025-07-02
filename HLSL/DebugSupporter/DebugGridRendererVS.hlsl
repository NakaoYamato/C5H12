#include "DebugRenderer.hlsli"

VS_OUT main(float3 position : POSITION, float4 vinColor : COLOR)
{
    VS_OUT vout;
    vout.position = mul(float4(position, 1.0f), worldViewProjection);
    vout.color = vinColor;

    return vout;
}