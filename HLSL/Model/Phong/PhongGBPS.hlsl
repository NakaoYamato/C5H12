#include "Phong.hlsli"

#include "../../GBuffer/GBuffer.hlsli"

#define POINT 0
#define LINEAR 1
#define ANISOTROPIC 2
SamplerState samplerStates[3] : register(s0);
Texture2D diffuseMap : register(t0);
Texture2D normalMap : register(t1);
Texture2D specularMap : register(t2);
Texture2D roughnessMap : register(t3);
Texture2D emissiveMap : register(t4);

// 環境マッピング
Texture2D environmentMap : register(t10);


PS_GB_OUT main(VS_OUT pin)
{
    float4 diffuseColor = diffuseMap.Sample(samplerStates[ANISOTROPIC], pin.texcoord) * Kd * pin.materialColor;
    // デザリング
    {
        //static const int dither_pattern[16] =
        //{
        //    0, 8, 2, 10,
        //    12, 4, 14, 6,
        //     3, 11, 1, 9,
        //    15, 7, 13, 5
        //};
        //uint x = ((uint) pin.position.x) % 4;
        //uint y = ((uint) pin.position.y) % 4;
        //float dither = (float) dither_pattern[x + y * 4] / 16.0f;
        //clip(diffuseColor.a - dither);
    }
    float4 emissiveColor = emissiveMap.Sample(samplerStates[ANISOTROPIC], pin.texcoord);
    float4 specularColor = specularMap.Sample(samplerStates[ANISOTROPIC], pin.texcoord) * Ks.rgba;
    float3x3 mat =
    {
        normalize(pin.world_tangent.xyz),
        normalize(pin.binormal.xyz),
        normalize(pin.world_normal.xyz)
    };
    float3 N = normalMap.Sample(samplerStates[ANISOTROPIC], pin.texcoord).rgb;
    // ノーマルテクスチャ法線をワールドへ変換
    N = normalize(mul(N * 2.0f - 1.0f, mat));
    
    //PS_GB_OUT pout = (PS_GB_OUT) 0;
    //pout.diffuseColor = diffuseColor;
    //pout.specularColor = specularColor;
    //pout.worldPosition = pin.world_position;
    //pout.worldNormal = float4(N, 1.0f);
    
    return CreateOutputData(
    diffuseColor.rgb,
    specularColor.x,
    N,
    0.5f,
    emissiveColor.rgb, 
    0.5f);
}