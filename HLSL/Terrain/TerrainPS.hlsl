#include "Terrain.hlsli"
#include "../Define/SamplerStateDefine.hlsli"
SamplerState samplerStates[_SAMPLER_STATE_MAX] : register(s0);

#include "../Model/Light.hlsli"
// 光源定数バッファ
cbuffer LIGHT_CONSTANT_BUFFER : register(b3)
{
    float4 world_ambient;
    float4 directional_light_direction;
    float4 directional_light_color;
    PointLight pointLights[8];
};
#include "../Function/ShadingFunctions.hlsli"

#define ROCK_COLOR 0
#define DIRT_COLOR 1
#define GRASS_COLOR 2
#define ROCK_NORMAL 3
#define DIRT_NORMAL 4
#define GRASS_NORMAL 5
Texture2D terrainTextures[6] : register(t0);

float4 main(PS_IN pin) : SV_TARGET
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
    float4 diffuseColor = rockColor * blendRate.x +
                   dirtColor * blendRate.y +
                   grassColor * blendRate.z +
                   baseColor * blendRate.a;
    float3 N = rockNormal * blendRate.x +
                    dirtNormal * blendRate.y +
                    grassNormal * blendRate.z +
                    float3(0, 1, 0) * blendRate.a;
    N = normalize(N);
    
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
    N = normalize(mul(N * 2.0f - 1.0f, mat));
    
    // フォンシェーディング用変数
    float3 E = normalize(pin.worldPosition.xyz - cameraPosition.xyz);
    float3 L = normalize(directional_light_direction.xyz);
    
    // ハーフランバート処理
    float3 directionalDiffuse = CalcHalfLambert(N, L, directional_light_color.rgb, baseColor.rgb);
    
    // 点光源の処理
    float3 pointDiffuse = 0;
    for (int i = 0; i < 8; ++i)
    {
        if (pointLights[i].isAlive != 1)
            continue;
        float3 LP = pin.worldPosition.xyz - pointLights[i].position.xyz;
        float len = length(LP);
        if (len >= pointLights[i].range)
            continue;
        float attenuateLength = saturate(1.0f - len / pointLights[i].range);
        float attenuation = attenuateLength * attenuateLength;
        LP /= len;
        pointDiffuse += CalcLambert(N, LP, pointLights[i].color.rgb, diffuseColor.rgb) * attenuation;
    }
    
    float4 color = float4(0.0f, 0.0f, 0.0f, diffuseColor.a);
    color.rgb += diffuseColor.rgb * saturate(world_ambient.rgb /*+ ambientColor.rgb*/ + directionalDiffuse + pointDiffuse);
    
    return color;
}