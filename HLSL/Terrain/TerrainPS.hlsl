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

Texture2D terrainColorTexture : register(t0);
Texture2D terrainNormalTexture : register(t1);

float4 main(PS_IN pin) : SV_TARGET
{
    // 色と法線を取得
    float4 diffuseColor = terrainColorTexture.Sample(samplerStates[_ANISOTROPIC_SAMPLER_INDEX], pin.texcoord);
    float3 normal = terrainNormalTexture.Sample(samplerStates[_ANISOTROPIC_SAMPLER_INDEX], pin.texcoord).rgb;
    float3 N = normalize(normal);
    
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
    float3 directionalDiffuse = CalcHalfLambert(N, L, directional_light_color.rgb, diffuseColor.rgb);
    
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