#include "PhysicalBasedRendering.hlsli"
#include "../../GBuffer/GBuffer.hlsli"
#define POINT 0
#define LINEAR 1
#define ANISOTROPIC 2
SamplerState samplerStates[3] : register(s0);

#define BASECOLOR_TEXTURE 0
#define ROUGHNESS_TEXTURE 1
#define NORMAL_TEXTURE 2
#define EMISSIVE_TEXTURE 3
Texture2D textureMaps[4] : register(t0);

PS_GB_OUT main(VS_OUT pin)
{
	// ベースカラー取得
    float4 baseColor = pin.materialColor;
    {
        float4 sampled = textureMaps[BASECOLOR_TEXTURE].Sample(samplerStates[ANISOTROPIC], pin.texcoord);
        sampled.rgb = pow(sampled.rgb, GammaFactor);
        baseColor *= sampled;
    }
	
	//	自己発光色取得
    float3 emissiveColor = textureMaps[EMISSIVE_TEXTURE].Sample(samplerStates[ANISOTROPIC], pin.texcoord).rgb;
    emissiveColor.rgb = pow(emissiveColor.rgb, GammaFactor);
    
    // 法線・従法線・接線取得
    float3 N = textureMaps[NORMAL_TEXTURE].Sample(samplerStates[ANISOTROPIC], pin.texcoord).rgb;
    // ノーマルテクスチャ法線をワールドへ変換
    float3x3 mat =
    {
        normalize(pin.world_tangent.xyz),
        normalize(pin.binormal.xyz),
        normalize(pin.world_normal.xyz)
    };
    N = normalize(mul(N * 2.0f - 1.0f, mat));
    
	//	金属質・粗さ取得
    float roughness = roughness_factor;
    float metalness = metalness_factor;
    {
        float4 sampled = textureMaps[ROUGHNESS_TEXTURE].Sample(samplerStates[LINEAR], pin.texcoord);
        roughness *= sampled.g;
    }
    
    return CreateOutputData(
    baseColor.rgb,
    0.0f,
    N,
    metalness,
    emissiveColor.rgb,
    roughness);
}