#include "Phong.hlsli"
#include "../../Define/SamplerStateDefine.hlsli"
SamplerState samplerStates[_SAMPLER_STATE_MAX] : register(s0);
Texture2D diffuseMap : register(t0);
Texture2D normalMap : register(t1);
Texture2D specularMap : register(t2);

// 環境マッピング
Texture2D environmentMap : register(t10);


float4 main(VS_OUT pin) : SV_TARGET
{
    float4 diffuseColor = diffuseMap.Sample(samplerStates[_LINEAR_WRAP_SAMPLER_INDEX], pin.texcoord) * Kd * pin.materialColor;
    if (diffuseColor.a < 0.01f)
        discard;
    float4 specularColor = specularMap.Sample(samplerStates[_LINEAR_WRAP_SAMPLER_INDEX], pin.texcoord) * Ks.rgba;
    
    // フォンシェーディング用変数
    float3 E = normalize(pin.world_position.xyz - cameraPosition.xyz);
    float3 L = normalize(directional_light_direction.xyz);
    float3x3 mat =
    {
        normalize(pin.world_tangent.xyz),
        normalize(pin.binormal.xyz),
        normalize(pin.world_normal.xyz)
    };
    //float3 N = normalize(pin.world_normal.xyz);
    float3 N = normalMap.Sample(samplerStates[_LINEAR_WRAP_SAMPLER_INDEX], pin.texcoord).rgb;
    // ノーマルテクスチャ法線をワールドへ変換
    N = normalize(mul(N * 2.0f - 1.0f, mat));
    
    // ハーフランバート処理
    float3 directionalDiffuse = CalcHalfLambert(N, L, directional_light_color.rgb, diffuseColor.rgb);
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
        pointDiffuse += CalcLambert(N, LP, pointLights[i].color.rgb, diffuseColor.rgb) * attenuation;
        pointSpecular += CalcPhongSpecular(N, LP, E, pointLights[i].color.rgb, specularColor.rgb) * attenuation;
    }
    
    float4 color = float4(0.0f, 0.0f, 0.0f, diffuseColor.a);
    color.rgb += diffuseColor.rgb * saturate(world_ambient.rgb + directionalDiffuse + pointDiffuse);
    color.rgb += directionalSpecular + pointSpecular;
    
    return color;
}