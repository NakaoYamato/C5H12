#include "../../Sprite/Sprite.hlsli"
#include "../../Scene/SceneConstantBuffer.hlsli"

SamplerState borderPointState : register(s4);
// CASCADED_SHADOW_MAPS
SamplerComparisonState comparison_sampler_state : register(s5);

Texture2D color_map : register(t0);
Texture2D depth_map : register(t1);
Texture2DArray cascaded_shadow_maps : register(t2);

cbuffer PARAMETRIC_CONSTANT_BUFFER : register(b2)
{
	// CASCADED_SHADOW_MAPS
    float shadow_color;
    float shadow_depth_bias;
    float colorize_cascaded_layer;
    float shadow_shift_volume;
};
cbuffer csm_constants : register(b3)
{
    row_major float4x4 cascaded_matrices[4];
    float4 cascaded_plane_distances;
}

float4 main(VsOut pin) : SV_TARGET
{
    float4 sampled_color = color_map.Sample(borderPointState, pin.texcoord);
    float3 color = sampled_color.rgb;
    float alpha = sampled_color.a;
	
    float depth_ndc = depth_map.Sample(borderPointState, pin.texcoord).x;

    float4 position_ndc;
	// texture space to ndc
    position_ndc.x = pin.texcoord.x * +2 - 1;
    position_ndc.y = pin.texcoord.y * -2 + 1;
    position_ndc.z = depth_ndc;
    position_ndc.w = 1;

	// ndc to view space
    float4 position_view_space = mul(position_ndc, inv_projection);
    position_view_space = position_view_space / position_view_space.w;
	
	// ndc to world space
    float4 position_world_space = mul(position_ndc, inv_view_projection);
    position_world_space = position_world_space / position_world_space.w;

	
	
	// Apply cascaded shadow mapping
	// Find a layer of cascaded view frustum volume 
    float depth_view_space = position_view_space.z;
    int cascade_index = -1;
    for (uint layer = 0; layer < 4; ++layer)
    {
        float distance = cascaded_plane_distances[layer];
        if (distance > depth_view_space)
        {
            cascade_index = layer;
            break;
        }
    }
    float shadow_factor = 1.0;
    if (cascade_index > -1)
    {
	
        // ハードシャドウのみ
		// world space to light view clip space, and to ndc
        float4 position_light_space = mul(position_world_space, cascaded_matrices[cascade_index]);
        position_light_space /= position_light_space.w;
		// ndc to texture space
        position_light_space.x = position_light_space.x * +0.5 + 0.5;
        position_light_space.y = position_light_space.y * -0.5 + 0.5;
        shadow_factor = cascaded_shadow_maps.SampleCmpLevelZero(comparison_sampler_state, float3(position_light_space.xy, cascade_index), position_light_space.z - shadow_depth_bias).x;
#if 1   // ソフトシャドウ
        const uint samples = 24;
        const float2 sampleOffsets[samples] =
        {
            float2(-1, +0), float2(+1, +0), float2(+0, +1), float2(+0, -1),
            float2(-1, -1), float2(-1, +1), float2(+1, -1), float2(+1, +1),
            float2(-2, +0), float2(+2, +0), float2(+0, +2), float2(+0, -2),
            float2(-2, -2), float2(-2, +2), float2(+2, -2), float2(+2, +2),
            float2(-3, +0), float2(+3, +0), float2(+0, +3), float2(+0, -3),
            float2(-3, -3), float2(-3, +3), float2(+3, -3), float2(+3, +3),
        };
        float2 shadowMapSize;
        uint elements;
        cascaded_shadow_maps.GetDimensions(shadowMapSize.x, shadowMapSize.y, elements);
        
        float shadowSum = 0.0;
	    [unroll]
        for (uint i = 0; i < samples; ++i)
        {
		    // world space to light view clip space, and to ndc
            float4 position_light_space = mul(position_world_space, cascaded_matrices[cascade_index]);
            position_light_space /= position_light_space.w;
		    // ndc to texture space
            position_light_space.x = position_light_space.x * +0.5 + 0.5;
            position_light_space.y = position_light_space.y * -0.5 + 0.5;
            position_light_space.x += sampleOffsets[i].x / shadow_shift_volume;
            position_light_space.y += sampleOffsets[i].y / shadow_shift_volume;
            float factor = cascaded_shadow_maps.SampleCmpLevelZero(comparison_sampler_state, float3(position_light_space.xy, cascade_index), position_light_space.z - shadow_depth_bias).x;
            if(factor > 0.0f)
                shadowSum += factor;
        }
        shadow_factor = saturate(shadow_factor + shadowSum / samples);
#endif        
        
        color *= lerp(shadow_color, 1.0, shadow_factor);
        
#if 1   // シャドウマップの各エリアを可視化
        if (colorize_cascaded_layer)
        {
            const float3 layer_colors[4] =
            {
                { 1, 0, 0 },
                { 0, 1, 0 },
                { 0, 0, 1 },
                { 1, 1, 0 },
            };
            color *= layer_colors[cascade_index];
        }
#endif        
    }
	
    return float4(color, alpha);
}