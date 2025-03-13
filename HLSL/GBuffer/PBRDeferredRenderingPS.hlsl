#include "../Sprite/Sprite.hlsli"

#include "../Scene/SceneConstantBuffer.hlsli"
#include "../Model/Light.hlsli"
#include "GBuffer.hlsli"
// 光源定数バッファ
cbuffer LIGHT_CONSTANT_BUFFER : register(b3)
{
    float4 world_ambient;
    float4 directional_light_direction;
    float4 directional_light_color;
    PointLight pointLights[8];
};

#include "../Function/ShadingFunctions.hlsli"

Texture2D textureMaps[TEXTURE_MAX] : register(t0);
SamplerState point_sampler_state : register(s0);
SamplerState linear_sampler_state : register(s2);

//  IBL用テクスチャ
TextureCube diffuse_iem : register(t11);
TextureCube specular_pmrem : register(t12);
Texture2D lut_ggx : register(t13);

//  gbufferに記録された深度値を出力する
struct PS_OUT
{
    float4 color : SV_TARGET;
    float depth : SV_DEPTH;
};

PS_OUT main(VsOut pin)
{
    // GBufferからデータを取得
    GBufferData decodeData = DecodeGBuffer(textureMaps, point_sampler_state, pin.texcoord, inv_view_projection);
    
    // decodeData.baseColorにアルファ値がないのでそのままだとskymapが埋もれてしまう
    // 対策として深度値からクリップしている
    clip(0.999999f - decodeData.depth);
    
	// ベースカラー取得
    float3 baseColor = decodeData.baseColor.rgb;
    
	//	自己発光色取得
    float3 emissiveColor = decodeData.emissiveColor.rgb;
    
    // 法線取得
    float3 N = decodeData.worldNormal.xyz;
    
	//	金属質・粗さ取得
    float roughness = decodeData.roughness;
    float metalness = decodeData.metallic;
    
	//	(非金属部分)
    float3 albedo = baseColor;
    
	//	入射光のうち拡散反射になる割合
    float3 diffuse_reflectance = lerp(albedo.rgb, 0.0f, metalness);
    
	//	垂直反射時のフレネル反射率(非金属でも最低4%は鏡面反射する)
    float3 F0 = lerp(0.04f, albedo.rgb, metalness);

	//	視線ベクトル
    float3 V = normalize(decodeData.worldPosition.xyz - camera_position.xyz);
    
	//	直接光のシェーディング
    float3 total_diffuse = 0, total_specular = 0;
	{
		// 平行光源の処理
		{
            float3 diffuse = (float3) 0, specular = (float3) 0;
            float3 L = normalize(directional_light_direction.xyz);
            DirectBRDF(diffuse_reflectance, F0, N, V, L,
					   directional_light_color.rgb * directional_light_color.a, roughness,
					   diffuse, specular);
            
            total_diffuse += diffuse;
            total_specular += specular;
        }

		//	点光源
        for (int i = 0; i < 8; ++i)
        {
            float3 L = decodeData.worldPosition.xyz - pointLights[i].position.xyz;
            float len = length(L);
            if (len >= pointLights[i].range)
                continue;
            float attenuateLength = saturate(1.0f - len / pointLights[i].range);
            float attenuation = attenuateLength * attenuateLength;
            L /= len;
            float3 diffuse = (float3) 0, specular = (float3) 0;
            DirectBRDF(diffuse_reflectance, F0, N, V, L,
					   pointLights[i].color.rgb * pointLights[i].color.a, roughness,
					   diffuse, specular);
            total_diffuse += diffuse * attenuation;
            total_specular += specular * attenuation;
        }
    }
    
    // IBL処理
    total_diffuse += DiffuseIBL(N, V, roughness, diffuse_reflectance, F0, diffuse_iem, linear_sampler_state);
    total_specular += SpecularIBL(N, V, roughness, F0, lut_ggx, specular_pmrem, linear_sampler_state);

	//	色生成
    float3 color = total_diffuse + total_specular + emissiveColor;    
    PS_OUT pout = (PS_OUT) 0;
    pout.color = float4(pow(color.rgb, 1.0f / GammaFactor), 1.0f);
    pout.depth = decodeData.depth;
    return pout;
}