#include "Terrain.hlsli"
#include "../GBuffer/GBuffer.hlsli"
#include "../Define/SamplerStateDefine.hlsli"
SamplerState samplerStates[_SAMPLER_STATE_MAX] : register(s0);

Texture2D terrainColorTexture : register(t0);
Texture2D terrainNormalTexture : register(t1);

PS_GB_OUT main(PS_IN pin)
{
    // 色と法線を取得
    float4 color = terrainColorTexture.Sample(samplerStates[_ANISOTROPIC_SAMPLER_INDEX], pin.texcoord);
    float3 normal = terrainNormalTexture.Sample(samplerStates[_ANISOTROPIC_SAMPLER_INDEX], pin.texcoord).rgb;
    normal = normalize(normal);
    
    // 法線計算
    float3 worldTangent = normalize(mul(float4(1.0f, 0.0f, 0.0f, 0.0f), world)).rgb;
    float3 binormal = normalize(mul(float4(0.0f, 0.0f, -1.0f, 0.0f), world)).rgb;
    // ノーマルテクスチャ法線をワールドへ変換
    float3x3 mat =
    {
        normalize(worldTangent),
        normalize(binormal),
        normalize(pin.normal)
    };
    normal = normalize(mul(normal * 2.0f - 1.0f, mat));
            
    return CreateOutputData(
    color.rgb,
    emissive,
    normal,
    metalness,
    float3(0.0f, 0.0f, 0.0f),
    roughness);
}