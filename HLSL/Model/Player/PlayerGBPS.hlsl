#include "../PhysicalBasedRendering/PhysicalBasedRendering.hlsli"

#include "../../GBuffer/GBuffer.hlsli"
#include "../../Function/Noise.hlsli"
#include "../../Define/SamplerStateDefine.hlsli"
SamplerState samplerStates[_SAMPLER_STATE_MAX] : register(s0);

#define BASECOLOR_TEXTURE 0
#define ROUGHNESS_TEXTURE 1
#define NORMAL_TEXTURE 2
#define EMISSIVE_TEXTURE 3
Texture2D textureMaps[4] : register(t0);

cbuffer CbPlayer : register(b4)
{
    float4 bodyColor;
}

PS_GB_OUT main(VS_OUT pin)
{
	// ベースカラー取得
    float4 baseColor = pin.materialColor * meshBaseColor;
    {
        float4 sampled = textureMaps[BASECOLOR_TEXTURE].Sample(samplerStates[_LINEAR_WRAP_SAMPLER_INDEX], pin.texcoord);
        sampled.rgb = pow(sampled.rgb, _GAMMA_FACTOR);
        baseColor *= sampled;
    }
    // デザリング
    Dithering(pin.position.xy, baseColor.a);
    
	//	自己発光色取得
    float3 emissiveColor = textureMaps[EMISSIVE_TEXTURE].Sample(samplerStates[_LINEAR_WRAP_SAMPLER_INDEX], pin.texcoord).rgb;
    emissiveColor.rgb = pow(emissiveColor.rgb, _GAMMA_FACTOR);
    
    // 法線・従法線・接線取得
    float3 N = textureMaps[NORMAL_TEXTURE].Sample(samplerStates[_LINEAR_WRAP_SAMPLER_INDEX], pin.texcoord).rgb;
    // ノーマルテクスチャ法線をワールドへ変換
    float3x3 mat =
    {
        normalize(pin.world_tangent.xyz),
        normalize(pin.binormal.xyz),
        normalize(pin.world_normal.xyz)
    };
    N = normalize(mul(N * 2.0f - 1.0f, mat));
    
	//	金属質・粗さ取得
    float roughness = roughnessFactor;
    float metalness = metalnessFactor;
    {
        float4 sampled = textureMaps[ROUGHNESS_TEXTURE].Sample(samplerStates[_LINEAR_WRAP_SAMPLER_INDEX], pin.texcoord);
        roughness *= sampled.g;
        metalness *= sampled.b;
    }
    
	//	視線ベクトル
    float3 V = normalize(pin.world_position.xyz - cameraPosition.xyz);
    
    // 視線方向に応じて色を加える
    baseColor.rgb += bodyColor.rgb * (1.0f - saturate(dot(N, -V))) * bodyColor.a * saturate(Noise(N));
    
    return CreateOutputData(
    baseColor.rgb,
    0.0f,
    N,
    metalness,
    emissiveColor.rgb,
    roughness,
    float4(decalMask, 0, 0, 0));
}