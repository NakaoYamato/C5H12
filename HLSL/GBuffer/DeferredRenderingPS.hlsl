#include "../Sprite/Sprite.hlsli"

#include "../CBuffer/B0/SceneCB.hlsli"
#include "../CBuffer/B3/LightCB.hlsli"

#include "GBuffer.hlsli"

#include "../Function/ShadingFunctions.hlsli"

Texture2D textureMaps[_TEXTURE_MAX] : register(t0);
#include "../Define/SamplerStateDefine.hlsli"
SamplerState samplerStates[_SAMPLER_STATE_MAX] : register(s0);

//  gbufferに記録された深度値を出力する
struct PS_OUT
{
    float4 color : SV_TARGET;
    float depth : SV_DEPTH;
};

PS_OUT main(VsOut pin)
{
    // GBufferからデータを取得
    GBufferData decodeData = DecodeGBuffer(textureMaps, samplerStates[_POINT_WRAP_SAMPLER_INDEX], pin.texcoord, invViewProjection);
    
    // decodeData.baseColorにアルファ値がないのでそのままだとskymapが埋もれてしまう
    // 対策として深度値からクリップしている
    clip(0.999999f - decodeData.depth);
    
    float4 specularColor = float4(decodeData.specular, decodeData.specular, decodeData.specular, 1.0f);
    // フォンシェーディング用変数
    float3 E = normalize(decodeData.worldPosition.xyz - cameraPosition.xyz);
    float3 L = normalize(directional_light_direction.xyz);
    
    // ハーフランバート処理
    float3 directionalDiffuse = CalcHalfLambert(decodeData.worldNormal, L, directional_light_color.rgb, decodeData.baseColor.rgb);
    float3 directionalSpecular = CalcPhongSpecular(decodeData.worldNormal, L, E, directional_light_color.rgb, specularColor.rgb);
    
    // 点光源の処理
    float3 pointDiffuse = 0;
    float3 pointSpecular = 0;
    for (int i = 0; i < 8; ++i)
    {
        if (pointLights[i].isAlive != 1)
            continue;
        float3 LP = decodeData.worldPosition.xyz - pointLights[i].position.xyz;
        float len = length(LP);
        if (len >= pointLights[i].range)
            continue;
        float attenuateLength = saturate(1.0f - len / pointLights[i].range);
        float attenuation = attenuateLength * attenuateLength;
        LP /= len;
        pointDiffuse += CalcLambert(decodeData.worldNormal, LP, pointLights[i].color.rgb, decodeData.baseColor.rgb) * attenuation;
        pointSpecular += CalcPhongSpecular(decodeData.worldNormal, LP, E, pointLights[i].color.rgb, specularColor.rgb) * attenuation;
    }
    
    float4 color = float4(decodeData.emissiveColor.r, decodeData.emissiveColor.g, decodeData.emissiveColor.b, 1.0f);
    color.rgb += decodeData.baseColor.rgb * saturate(world_ambient.rgb /*+ ambientColor.rgb*/ + directionalDiffuse + pointDiffuse);
    color.rgb += directionalSpecular + pointSpecular;
    // リムライト処理
    // TODO : RimPower
    //color.rgb += CalcRimLight(worldNormal, E, L, directional_light_color.rgb);
    
    PS_OUT pout = (PS_OUT) 0;
    pout.color = color;
    pout.depth = decodeData.depth;
    return pout;
}