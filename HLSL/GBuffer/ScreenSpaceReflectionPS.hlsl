#include "../Sprite/Sprite.hlsli"
#include "../CBuffer/B0/SceneCB.hlsli"

cbuffer SSR_BUFFER : register(b6)
{
    // 反射強度
    float refrectionIntensity;
    float maxDistance = 15.0f;
    float resolution = 0.3f;
    float thickness = 0.5f;
}

Texture2D colorMap : register(t0);
Texture2D depthMap : register(t1);
Texture2D normalMap : register(t2);
#include "../Define/SamplerStateDefine.hlsli"
SamplerState samplerStates[_SAMPLER_STATE_MAX] : register(s0);

float2 NDC2UV(float2 ndc)
{
    float2 uv;
    uv.x = 0.5 + 0.5 * ndc.x;
    uv.y = 0.5 - ndc.y * 0.5;
    return uv;
}

inline float FSchlick(float f0, float cos)
{
    return f0 + (1 - f0) * pow(1 - cos, 5);
}

float4 GetViewSpacePosition(float2 uv)
{
    // UV 座標から深度値を取得
    float depth = depthMap.Sample(samplerStates[_POINT_WRAP_SAMPLER_INDEX], uv).r;
    
    float4 position = float4(uv.x * 2.0f - 1.0f, uv.y * -2.0f + 1.0f, depth, 1);
    position = mul(position, invViewProjection);
    float3 worldPosition = position.xyz;
    
    return mul(float4(worldPosition.xyz, 1.0f), view);
}

float3 GetViewSpaceNormal(float2 uv)
{
    // ワールド空間ノーマル取得
    float3 worldNormal = normalMap.Sample(samplerStates[_POINT_WRAP_SAMPLER_INDEX], uv).xyz;
    
    // ビュー空間法線化
    float4 viewNormal = mul(float4(worldNormal, 0.0f), view);
    
    return viewNormal.xyz;
}

float4 main(VsOut pin) : SV_TARGET
{
    int steps = 10;
	// https://lettier.github.io/3d-game-shaders-for-beginners/screen-space-reflection.html
    
	// Place them in a constant buffer in the future so that you can control them.

    
    uint2 dimensions;
    uint mip_level = 0, number_of_levels;
    depthMap.GetDimensions(mip_level, dimensions.x, dimensions.y, number_of_levels);
    
    float4 position = GetViewSpacePosition(pin.texcoord); // view-space
    float3 normal = GetViewSpaceNormal(pin.texcoord); // view-space
    
    float4 position_from = position;
    float4 position_to = position_from;
	
    float3 incident = normalize(position_from.xyz);
    float3 reflection = normalize(reflect(incident, normal.xyz));
	
    float4 start_view = float4(position_from.xyz + (reflection * 0), 1);
    float4 end_view = float4(position_from.xyz + (reflection * maxDistance), 1);
	
    float4 start_frag = mul(start_view, projection); // from view to clip-space
    start_frag /= start_frag.w; // from clip-space to ndc
    start_frag.xy = saturate(NDC2UV(start_frag.xy)); // from uv to fragment/pixel coordinates
    start_frag.xy *= dimensions;
	
    float4 end_frag = mul(end_view, projection); // from view to clip-space
    end_frag /= end_frag.w; // from clip-space to ndc
    end_frag.xy = saturate(NDC2UV(end_frag.xy)); // from ndc to uv
    end_frag.xy *= dimensions;
    
    float2 frag = start_frag.xy;
	
    float4 uv = 0;
    uv.xy = frag / dimensions;
	
    float delta_x = end_frag.x - start_frag.x;
    float delta_y = end_frag.y - start_frag.y;
	
    float use_x = abs(delta_x) >= abs(delta_y) ? 1 : 0;
    float delta = lerp(abs(delta_y), abs(delta_x), use_x) * clamp(resolution, 0, 1);

    float2 increment = float2(delta_x, delta_y) / max(delta, 0.001);
	
    float search_0 = 0;
    float search_1 = 0;

    int hit_0 = 0;
    int hit_1 = 0;

    float view_distance = start_view.z;
    float depth = thickness;
    
    int i = 0;
	[unroll(128)]
    for (i = 0; i < (int) delta /* && any(normal.xyz) && position_to.w > 0*/; ++i)
    {
        frag += increment;
        uv.xy = frag / dimensions;
 
        position_to = GetViewSpacePosition(uv.xy); // view-space

        search_1 = lerp((frag.y - start_frag.y) / delta_y, (frag.x - start_frag.x) / delta_x, use_x);
        search_1 = clamp(search_1, 0.0, 1.0);

		// Perspective-Correct Interpolation
		// https://www.comp.nus.edu.sg/~lowkl/publications/lowk_persp_interp_techrep.pdf
        view_distance = (start_view.z * end_view.z) / lerp(end_view.z, start_view.z, search_1);

        depth = view_distance - position_to.z;

        if (depth > 0 && depth < thickness)
        {
            hit_0 = 1;
            break;
        }
        else
        {
            search_0 = search_1;
        }
    }
#if 0
	hit_1 = hit_0;
#else
    search_1 = search_0 + ((search_1 - search_0) / 2.0);
    steps *= hit_0;
    
	[unroll]
    for (i = 0; i < steps; ++i)
    {
        frag = lerp(start_frag.xy, end_frag.xy, search_1);
        uv.xy = frag / dimensions;
		
        position_to = GetViewSpacePosition(uv.xy); // view-space

		// Perspective-Correct Interpolation
		// https://www.comp.nus.edu.sg/~lowkl/publications/lowk_persp_interp_techrep.pdf
        view_distance = (start_view.z * end_view.z) / lerp(end_view.z, start_view.z, search_1);
		
        depth = view_distance - position_to.z;

        if (depth > 0 && depth < thickness)
        {
            hit_1 = 1;
            search_1 = search_0 + ((search_1 - search_0) / 2);
        }
        else
        {
            float temp = search_1;
            search_1 = search_1 + ((search_1 - search_0) / 2);
            search_0 = temp;
        }
    }
#endif
	
    float visibility = hit_1;
    visibility *= (1 - max(dot(-normalize(position_from.xyz), reflection), 0));
    visibility *= (1 - clamp(depth / thickness, 0, 1));
    visibility = clamp(visibility, 0, 1);
    visibility *= position_to.w;
    visibility *= (uv.x < 0 || uv.x > 1 || uv.y < 0 || uv.y > 1) ? 0 : 1;

    float fresnel = saturate(FSchlick(0.04, max(0, dot(reflection, normal.xyz))));
    float3 reflection_color = colorMap.Sample(samplerStates[_POINT_WRAP_SAMPLER_INDEX], uv.xy).rgb;
    reflection_color = fresnel * reflection_color * visibility * refrectionIntensity;
	
    float4 color = colorMap.Sample(samplerStates[_POINT_WRAP_SAMPLER_INDEX], pin.texcoord);
    
#if 1
	// Gamma process
    const float GAMMA = 2.2;
    color.rgb = pow(color.rgb, GAMMA);
    
	// Apply reflection color
    color.rgb += reflection_color;
	
    color.rgb = pow(color.rgb, 1.0 / GAMMA);
#endif

    return float4(color.rgb, 1.0f);
}