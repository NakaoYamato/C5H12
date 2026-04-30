#include "TerrainGrass.hlsli"

#include "../../GBuffer/GBuffer.hlsli"

#include "../../Define/SamplerStateDefine.hlsli"
SamplerState samplerStates[_SAMPLER_STATE_MAX] : register(s0);

Texture2D colorMap : register(t4);
Texture2D normalMap : register(t5);

PS_GB_OUT main(GRASS_PS_IN pin)
{
    float3 color = colorMap.Sample(samplerStates[_ANISOTROPIC_SAMPLER_INDEX], pin.texcoord).rgb * pin.color.rgb;
    float3 normal = normalMap.Sample(samplerStates[_ANISOTROPIC_SAMPLER_INDEX], pin.texcoord).rgb;
    normal = normalize(normal);
    
    // 法線計算
    float3 binormal = normalize(cross(pin.worldNormal.xyz, pin.worldTangent.xyz));
    // ノーマルテクスチャ法線をワールドへ変換
    float3x3 mat =
    {
        normalize(pin.worldTangent.xyz),
        normalize(binormal),
        normalize(pin.worldNormal.xyz)
    };
    normal = normalize(mul(normal * 2.0f - 1.0f, mat));
    
    return CreateOutputData(
    color.rgb,
    0.5f,
    pin.worldNormal,
    0.5f,
    float3(0.0f, 0.0f, 0.0f),
    0.5f);
}