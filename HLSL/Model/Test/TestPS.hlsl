#include "../ModelInputLayout.hlsli"

#include "../Light.hlsli"

// 光源定数バッファ
cbuffer LIGHT_CONSTANT_BUFFER : register(b3)
{
    float4 world_ambient;
    float4 directional_light_direction;
    float4 directional_light_color;
    PointLight pointLights[8];
};

#include "../../Function/ShadingFunctions.hlsli"

float4 main(VS_OUT pin) : SV_TARGET
{
	// ベースを白で処理
    float4 baseColor = float4(1.0f, 1.0f, 1.0f, 1.0f);
    
    float3 N = normalize(pin.world_normal.xyz);
    float roughness = 0.5f;
    float metalness = 0.5f;
    
	//	入射光のうち拡散反射になる割合
    float3 diffuseReflectance = lerp(baseColor.rgb, 0.0f, metalness);
    
	//	垂直反射時のフレネル反射率(非金属でも最低4%は鏡面反射する)
    float3 F0 = lerp(0.04f, baseColor.rgb, metalness);
    
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
    
	//	色生成
    float3 color = totalDiffuse + totalSpecular;
    return float4(pow(color.rgb, 1.0f / _GAMMA_FACTOR), baseColor.a);
}