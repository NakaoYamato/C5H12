#include "Phong.hlsli"
#define POINT 0
#define LINEAR 1
#define ANISOTROPIC 2
SamplerState samplerStates[3] : register(s0);
Texture2D diffuseMap : register(t0);
Texture2D normalMap : register(t1);
Texture2D specularMap : register(t2);

// 環境マッピング
Texture2D environmentMap : register(t10);


float4 main(VS_OUT pin) : SV_TARGET
{
    float4 diffuseColor = diffuseMap.Sample(samplerStates[ANISOTROPIC], pin.texcoord) * pin.materialColor;
    diffuseColor.a = pin.materialColor.a;
    // TODO 処理が重い
    //clip(diffuseColor.a < 0.1f ? -1 : 1);
    float4 specularColor = specularMap.Sample(samplerStates[ANISOTROPIC], pin.texcoord) * Ks.rgba;
    
    // TODO : DiffuseColor alpha 
    float3 ambient = ambient_color.rgb * Ka.rgb;
    
    // フォンシェーディング用変数
    float3 E = normalize(pin.world_position.xyz - camera_position.xyz);
    float3 L = normalize(directional_light_direction.xyz);
    float3x3 mat = { 
        normalize(pin.world_tangent.xyz), 
        normalize(pin.binormal.xyz), 
        normalize(pin.world_normal.xyz) };
    //float3 N = normalize(pin.world_normal.xyz);
    float3 N = normalMap.Sample(samplerStates[ANISOTROPIC], pin.texcoord).rgb;
    // ノーマルテクスチャ法線をワールドへ変換
    N = normalize(mul(N * 2.0f - 1.0f, mat));
    
    // ハーフランバート処理
    float3 directionalDiffuse = CalcHalfLambert(N, L, directional_light_color.rgb, Kd.rgb);
    float3 directionalSpecular = CalcPhongSpecular(N, L, E, directional_light_color.rgb, specularColor.rgb);
    
    // 点光源の処理
    float3 pointDiffuse = 0;
    float3 pointSpecular = 0;
    for (int i = 0; i < 8; ++i)
    {
        if (pointLights[i].isAlive != 1)
            continue;
        float3 LP = pin.world_position.xyz - pointLights[i].position.xyz;
        float len = length(LP);
        if (len >= pointLights[i].range)
            continue;
        float attenuateLength = saturate(1.0f - len / pointLights[i].range);
        float attenuation = attenuateLength * attenuateLength;
        LP /= len;
        pointDiffuse += CalcLambert(N, LP, pointLights[i].color.rgb, Kd.rgb) * attenuation;
        pointSpecular += CalcPhongSpecular(N, LP, E, pointLights[i].color.rgb, Ks.rgb) * attenuation;
    }
    
    float4 color = float4(ambient, pin.materialColor.a * Kd.a * diffuseColor.a);
    color.rgb += (diffuseColor.rgb * (directionalDiffuse + pointDiffuse));
    color.rgb += directionalSpecular + pointSpecular;
    
    // リムライト処理
    color.rgb += CalcRimLight(N, E, L, directional_light_color.rgb);
    
    // 環境マップ処理
    //color.rgb = CalcSphereEnvironment(environmentMap, samplerStates[ANISOTROPIC],
				//					color.rgb, N, E, 0.0f);
    
    return color;
}