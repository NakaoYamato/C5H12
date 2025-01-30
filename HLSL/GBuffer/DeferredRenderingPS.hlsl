#include "../Sprite/Sprite.hlsli"

#include "../Scene/SceneConstantBuffer.hlsli"
#include "../Model/Light.hlsli"
#include "GBuffer.hlsli"
// 光源定数バッファ
cbuffer LIGHT_CONSTANT_BUFFER : register(b3)
{
    float4 ambient_color;
    float4 directional_light_direction;
    float4 directional_light_color;
    PointLight pointLights[8];
};

#include "../Function/ShadingFunctions.hlsli"

Texture2D textureMaps[TEXTURE_MAX] : register(t0);
SamplerState point_sampler_state : register(s0);
SamplerState linear_sampler_state : register(s2);

//  gbufferに記録された深度値を出力する
struct PS_OUT
{
    float4 color : SV_TARGET;
    float depth : SV_DEPTH;
};

PS_OUT main(VsOut pin)
{
    float4 diffuseColor     = textureMaps[DIFFUSE_COLOR_TEXTURE].Sample(point_sampler_state, pin.texcoord);
    float4 ambientColor     = textureMaps[AMBIENT_COLOR_TEXTURE].Sample(point_sampler_state, pin.texcoord);
    float4 specularColor    = textureMaps[SPECULAR_COLOR_TEXTURE].Sample(point_sampler_state, pin.texcoord);
    float3 worldPosition    = textureMaps[WORLD_POSITION_TEXTURE].Sample(point_sampler_state, pin.texcoord).xyz;
    float3 worldNormal      = textureMaps[WORLD_NORMAL_TEXTURE].Sample(point_sampler_state, pin.texcoord).xyz;
    float  depth            = textureMaps[DEPTH_TEXTURE].Sample(point_sampler_state, pin.texcoord).x;
    
    // フォンシェーディング用変数
    float3 E = normalize(worldPosition.xyz - camera_position.xyz);
    float3 L = normalize(directional_light_direction.xyz);
    
    // ハーフランバート処理
    float3 directionalDiffuse = CalcHalfLambert(worldNormal, L, directional_light_color.rgb, diffuseColor.rgb);
    float3 directionalSpecular = CalcPhongSpecular(worldNormal, L, E, directional_light_color.rgb, specularColor.rgb);
    
    // 点光源の処理
    float3 pointDiffuse = 0;
    float3 pointSpecular = 0;
    for (int i = 0; i < 8; ++i)
    {
        if (pointLights[i].isAlive != 1)
            continue;
        float3 LP = worldPosition.xyz - pointLights[i].position.xyz;
        float len = length(LP);
        if (len >= pointLights[i].range)
            continue;
        float attenuateLength = saturate(1.0f - len / pointLights[i].range);
        float attenuation = attenuateLength * attenuateLength;
        LP /= len;
        pointDiffuse += CalcLambert(worldNormal, LP, pointLights[i].color.rgb, diffuseColor.rgb) * attenuation;
        pointSpecular += CalcPhongSpecular(worldNormal, LP, E, pointLights[i].color.rgb, specularColor.rgb) * attenuation;
    }
    
    float4 color = ambientColor;
    color.rgb += (diffuseColor.rgb * (directionalDiffuse + pointDiffuse));
    color.rgb += directionalSpecular + pointSpecular;
    // リムライト処理
    color.rgb += CalcRimLight(worldNormal, E, L, directional_light_color.rgb);
    
    PS_OUT pout = (PS_OUT) 0;
    pout.color = color;
    pout.depth = depth;
    return pout;

}