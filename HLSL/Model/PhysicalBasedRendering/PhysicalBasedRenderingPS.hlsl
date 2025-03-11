#include "PhysicalBasedRendering.hlsli"
#define POINT 0
#define LINEAR 1
#define ANISOTROPIC 2
SamplerState samplerStates[3] : register(s0);

#define BASECOLOR_TEXTURE 0
#define ROUGHNESS_TEXTURE 1
#define NORMAL_TEXTURE 2
#define EMISSIVE_TEXTURE 3
Texture2D textureMaps[4] : register(t0);

float4 main(VS_OUT pin) : SV_TARGET
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
    
	//	(非金属部分)
    float4 albedo = baseColor;
    
	//	入射光のうち拡散反射になる割合
    float3 diffuse_reflectance = lerp(albedo.rgb, 0.0f, metalness);
    
	//	垂直反射時のフレネル反射率(非金属でも最低4%は鏡面反射する)
    float3 F0 = lerp(0.04f, albedo.rgb, metalness);

	//	視線ベクトル
    float3 V = normalize(pin.world_position.xyz - camera_position.xyz);
    
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
            float3 L = pin.world_position.xyz - pointLights[i].position.xyz;
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
    
	//	色生成
    float3 color = total_diffuse + total_specular + emissiveColor;
    return float4(pow(color.rgb, 1.0f / GammaFactor), baseColor.a);
}