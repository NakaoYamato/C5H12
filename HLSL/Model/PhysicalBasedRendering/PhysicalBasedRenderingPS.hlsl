#include "PhysicalBasedRendering.hlsli"
#include "../../Define/SamplerStateDefine.hlsli"
SamplerState samplerStates[_SAMPLER_STATE_MAX] : register(s0);

#define BASECOLOR_TEXTURE 0
#define ROUGHNESS_TEXTURE 1
#define NORMAL_TEXTURE 2
#define EMISSIVE_TEXTURE 3
Texture2D textureMaps[4] : register(t0);

//  IBL用テクスチャ
TextureCube diffuse_iem : register(t11);
TextureCube specular_pmrem : register(t12);
Texture2D lut_ggx : register(t13);

float4 main(VS_OUT pin) : SV_TARGET
{
	// ベースカラー取得
    float4 baseColor = pin.materialColor * meshBaseColor;
    {
        float4 sampled = textureMaps[BASECOLOR_TEXTURE].Sample(samplerStates[_LINEAR_WRAP_SAMPLER_INDEX], pin.texcoord);
        sampled.rgb = pow(sampled.rgb, _GAMMA_FACTOR);
        baseColor *= sampled;
    }
	
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
    
	//	(非金属部分)
    float4 albedo = baseColor;
    
	//	入射光のうち拡散反射になる割合
    float3 diffuseReflectance = lerp(albedo.rgb, 0.0f, metalness);
    
	//	垂直反射時のフレネル反射率(非金属でも最低4%は鏡面反射する)
    float3 F0 = lerp(0.04f, albedo.rgb, metalness);

	//	視線ベクトル
    float3 V = normalize(pin.world_position.xyz - cameraPosition.xyz);
    
	//	直接光のシェーディング
    float3 totalDiffuse = 0, totalSpecular = 0;
	{
		// 平行光源の処理
		{
            float3 diffuse = (float3) 0, specular = (float3) 0;
            float3 L = normalize(directional_light_direction.xyz);
            DirectBRDF(diffuseReflectance, F0, N, V, L,
					   directional_light_color.rgb * directional_light_color.a, roughness,
					   diffuse, specular);
            
            totalDiffuse += diffuse;
            totalSpecular += specular;
        }

		//	点光源
        for (int i = 0; i < 8; ++i)
        {
            float3 L = pin.world_position.xyz - pointLights[i].position.xyz;
            float len = length(L);
            if (len >= pointLights[i].range)
                continue;
            float attenuateLength = saturate(1.0f - len / pointLights[i].range);
            float attenuation = attenuateLength * attenuateLength;
            L /= len;
            float3 diffuse = (float3) 0, specular = (float3) 0;
            DirectBRDF(diffuseReflectance, F0, N, V, L,
					   pointLights[i].color.rgb * pointLights[i].color.a, roughness,
					   diffuse, specular);
            totalDiffuse += diffuse * attenuation;
            totalSpecular += specular * attenuation;
        }
    }
    
    // IBL処理
    totalDiffuse += DiffuseIBL(N, V, roughness, diffuseReflectance, F0, diffuse_iem, samplerStates[_LINEAR_WRAP_SAMPLER_INDEX]);
    totalSpecular += SpecularIBL(N, V, roughness, F0, lut_ggx, specular_pmrem, samplerStates[_LINEAR_WRAP_SAMPLER_INDEX]);

	//	色生成
    float3 color = totalDiffuse + totalSpecular + emissiveColor;
    return float4(pow(color.rgb, 1.0f / _GAMMA_FACTOR), baseColor.a);
}