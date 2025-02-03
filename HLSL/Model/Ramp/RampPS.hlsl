#include "../Phong/Phong.hlsli"
#define POINT 0
#define LINEAR 1
#define ANISOTROPIC 2
SamplerState samplerStates[3] : register(s0);
Texture2D diffuseMap : register(t0);
Texture2D normalMap : register(t1);

Texture2D rampMap : register(t5);

// 環境マッピング
Texture2D environmentMap : register(t10);

float4 main(VS_OUT pin) : SV_TARGET
{
    float4 diffuseColor = diffuseMap.Sample(samplerStates[ANISOTROPIC], pin.texcoord) * pin.materialColor;
    // TODO 処理が重い
    clip(diffuseColor.a < 0.1f ? -1 : 1);
	
    float3 E = normalize(pin.world_position.xyz - camera_position.xyz);
    float3 L = normalize(directional_light_direction.xyz);
    float3x3 mat =
    {
        normalize(pin.world_tangent.xyz),
        normalize(pin.binormal.xyz),
        normalize(pin.world_normal.xyz)
    };
    float3 N = normalMap.Sample(samplerStates[ANISOTROPIC], pin.texcoord).rgb;
    // ノーマルテクスチャ法線をワールドへ変換
    N = normalize(mul(N * 2.0f - 1.0f, mat));
    
    float3 ambient = world_ambient.rgb * Ka.rgb;
    
    float3 directionalDiffuse = CalcRampShading(rampMap, samplerStates[ANISOTROPIC],
    N, L, directional_light_color.rgb, Kd.rgb);
    
    float3 directionalSpecular = CalcPhongSpecular(N, L, E, directional_light_color.rgb, Ks.rgb);
    float4 color = float4(ambient, diffuseColor.a);
    color.rgb += diffuseColor.rgb * directionalDiffuse;
    color.rgb += directionalSpecular;
    return color;
}