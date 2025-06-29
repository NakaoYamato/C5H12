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
// ハイトマップ
Texture2D<float4> heightTextures : register(t6);

PS_GB_OUT main(PS_IN pin)
{
    // ブレンドしたいテクスチャの色を取得
    float2 tilling_coord = pin.texcoord * tillingScale;
    float4 rock_color = terrainTextures[ROCK_COLOR].Sample(samplerStates[_ANISOTROPIC_SAMPLER_INDEX], tilling_coord);
    float4 dirt_color = terrainTextures[DIRT_COLOR].Sample(samplerStates[_ANISOTROPIC_SAMPLER_INDEX], tilling_coord);
    float4 grass_color = terrainTextures[GRASS_COLOR].Sample(samplerStates[_ANISOTROPIC_SAMPLER_INDEX], tilling_coord);
    // ブレンド取得
    float blend_rate = saturate(heightTextures.Sample(samplerStates[_ANISOTROPIC_SAMPLER_INDEX], pin.texcoord).g);
    float4 color = lerp(rock_color, dirt_color, smoothstep(0.0f, 0.5f, blend_rate));
    color = lerp(color, grass_color, smoothstep(0.5f, 1.0f, blend_rate));
    float3 rockNormal = terrainTextures[ROCK_NORMAL].Sample(samplerStates[_ANISOTROPIC_SAMPLER_INDEX], tilling_coord).rgb;
    float3 dirtNormal = terrainTextures[DIRT_NORMAL].Sample(samplerStates[_ANISOTROPIC_SAMPLER_INDEX], tilling_coord).rgb;
    float3 grassNormal = terrainTextures[GRASS_NORMAL].Sample(samplerStates[_ANISOTROPIC_SAMPLER_INDEX], tilling_coord).rgb;
    float3 normal = normalize(lerp(rockNormal, dirtNormal, smoothstep(0.0f, 0.5f, blend_rate)));
    
    // 法線計算
    float3 worldTangent = normalize(mul(float4(1.0f, 0.0f, 0.0f, 0.0f), world)).rgb;
    float3 binormal = normalize(mul(float4(0.0f, 0.0f, -1.0f, 0.0f), world)).rgb;
    float3 worlNormal = normalize(mul(float4(0.0f, 1.0f, 0.0f, 0.0f), world)).rgb;
    // ノーマルテクスチャ法線をワールドへ変換
    float3x3 mat =
    {
        normalize(float3(1.0f, 0.0f, 0.0f)),
        normalize(float3(0.0f, 0.0f, -1.0f)),
        normalize(float3(0.0f, 1.0f, 0.0f))
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