#include "Grass.hlsli"

#include "../../GBuffer/GBuffer.hlsli"

// スケルトン用定数バッファ
cbuffer CbSkeleton : register(b1)
{
    float4 materialColor;
    row_major float4x4 worldTransform;
}

PS_GB_OUT main(GS_OUT pin)
{
    float3 L = directional_light_direction.xyz;
    float3 N = normalize(pin.normal.xyz);
    float diffuse_factor = max(0, dot(N, L) * 0.5 + 0.5);
    const float3 root_color = float3(0.2, 0.1, 0);
    float4 diffuseColor = float4(lerp(materialColor.rgb, root_color, pin.texcoord.y) * diffuse_factor, 1);
    
    return CreateOutputData(
    diffuseColor.rgb,
    0.0f,
    N,
    0.0f,
    float3(0.0f, 0.0f, 0.0f),
    0.0f);
}
