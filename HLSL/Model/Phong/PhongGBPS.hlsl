#include "Phong.hlsli"

#include "../../GBuffer/GBuffer.hlsli"

#define POINT 0
#define LINEAR 1
#define ANISOTROPIC 2
SamplerState samplerStates[3] : register(s0);
Texture2D diffuseMap : register(t0);
Texture2D normalMap : register(t1);
Texture2D specularMap : register(t2);

// 環境マッピング
Texture2D environmentMap : register(t10);


PS_GB_OUT main(VS_OUT pin)
{
    float4 diffuseColor = diffuseMap.Sample(samplerStates[ANISOTROPIC], pin.texcoord) * pin.materialColor;
    // デザリング
    {
        static const int dither_pattern[16] =
        {
            0, 8, 2, 10,
            12, 4, 14, 6,
             3, 11, 1, 9,
            15, 7, 13, 5
        };
        uint x = ((uint) pin.position.x) % 4;
        uint y = ((uint) pin.position.y) % 4;
        float dither = (float) dither_pattern[x + y * 4] / 16.0f;
        clip(diffuseColor.a - dither);
    }
    float4 specularColor = specularMap.Sample(samplerStates[ANISOTROPIC], pin.texcoord) * Ks.rgba;
    float4 ambientColor = Ka;
    float3x3 mat =
    {
        normalize(pin.world_tangent.xyz),
        normalize(pin.binormal.xyz),
        normalize(pin.world_normal.xyz)
    };
    float3 N = normalMap.Sample(samplerStates[ANISOTROPIC], pin.texcoord).rgb;
    // ノーマルテクスチャ法線をワールドへ変換
    N = normalize(mul(N * 2.0f - 1.0f, mat));
    
    PS_GB_OUT pout = (PS_GB_OUT) 0;
    pout.diffuseColor = diffuseColor;
    pout.ambientColor = ambientColor;
    pout.specularColor = specularColor;
    pout.worldPosition = pin.world_position;
    pout.worldNormal = float4(N, 0.0f);
    float4 position = mul(float4(pin.world_position.xyz, 1.0f), view_projection);
    pout.depth = position.z / position.w;
    
    return pout;
}