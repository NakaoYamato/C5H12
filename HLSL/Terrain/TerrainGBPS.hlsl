#include "Terrain.hlsli"
#include "../GBuffer/GBuffer.hlsli"
#include "../Define/SamplerStateDefine.hlsli"
SamplerState samplerStates[_SAMPLER_STATE_MAX] : register(s0);

#define ROCK_COLOR 0
#define DIRT_COLOR 1
#define GRASS_COLOR 2
#define ROCK_NORMAL 3
#define DIRT_NORMAL 4
#define GRASS_NORMAL 5
Texture2D terrainTextures[6] : register(t0);

PS_GB_OUT main(PS_IN pin)
{
    // 各テクスチャ情報取得
    float2 tillingCoord = pin.texcoord * tillingScale;
    float4 rockColor = terrainTextures[ROCK_COLOR].Sample(samplerStates[_ANISOTROPIC_SAMPLER_INDEX], tillingCoord);
    float4 dirtColor = terrainTextures[DIRT_COLOR].Sample(samplerStates[_ANISOTROPIC_SAMPLER_INDEX], tillingCoord);
    float4 grassColor = terrainTextures[GRASS_COLOR].Sample(samplerStates[_ANISOTROPIC_SAMPLER_INDEX], tillingCoord);
    float3 rockNormal = terrainTextures[ROCK_NORMAL].Sample(samplerStates[_ANISOTROPIC_SAMPLER_INDEX], tillingCoord).rgb;
    float3 dirtNormal = terrainTextures[DIRT_NORMAL].Sample(samplerStates[_ANISOTROPIC_SAMPLER_INDEX], tillingCoord).rgb;
    float3 grassNormal = terrainTextures[GRASS_NORMAL].Sample(samplerStates[_ANISOTROPIC_SAMPLER_INDEX], tillingCoord).rgb;
    
    // ブレンド率から最終的な色と法線を計算
    float4 blendRate = normalize(pin.blendRate);
    float4 color = rockColor * blendRate.x +
                   dirtColor * blendRate.y +
                   grassColor * blendRate.z +
                   baseColor * blendRate.a;
    float3 normal = rockNormal * blendRate.x +
                    dirtNormal * blendRate.y +
                    grassNormal * blendRate.z +
                    float3(0, 1, 0) * blendRate.a;
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