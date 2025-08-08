#include "../../Sprite/Sprite.hlsli"
#include "FilterFunctions.hlsli"

#include "../../Scene/SceneConstantBuffer.hlsli"

inline float Random(float2 n)
{
    return frac(sin(dot(n, float2(12.9898, 4.1414))) * 43758.5453);
}
inline float Noise(float2 p)
{
    float2 ip = floor(p);
    float2 u = frac(p);
    u = u * u * (3.0 - 2.0 * u);
	
    float res = lerp(
		lerp(Random(ip), Random(ip + float2(1.0, 0.0)), u.x),
		lerp(Random(ip + float2(0.0, 1.0)), Random(ip + float2(1.0, 1.0)), u.x), u.y);
    return res * res;
}

cbuffer CONSTANT_BUFFER : register(b1)
{
    // カラーフィルター
    float hueShift; // 色相調整
    float saturation; // 彩度調整
    float brightness; // 明度調整
    float cfDummy;
    
    // トーンマッピング
    float exposure; // 補正値
    int toneType; // 使用するトーンタイプ
    float2 tmDummy;
    
    // ヴィネット
    float4 vignetteColor;

    float2 vignetteCenter;
    float vignetteIntensity;
    float vignetteSmoothness;

    int vignetteRounded; // 1 : use  0 : unUse
    float vignetteRoundness;
    float vignetteDummy[2];
};

float3 cc(float3 color, float factor, float factor2) // color modifier
{
    float w = color.x + color.y + color.z;
    return lerp(color, w * factor, w * factor2);
}
// https://www.shadertoy.com/view/4sX3Rs
float3 lens_flare(float2 uv, float2 pos)
{
#if 1
    const float glory_light_intensity = 1.5;
    const float lens_flare_intensity = 3.0;

    float2 main = uv - pos;
    float2 uvd = uv * (length(uv));

    float ang = atan2(main.x, main.y);
    float dist = length(main);
    dist = pow(dist, .1);
#if 1
    //float n = noise(float2(ang * 16.0, dist * 32.0));
#else
	float n = noise_map.Sample(sampler_states[LINEAR], float2(ang * 16.0, dist * 32.0)).x;
#endif

	// Glory light
    float f0 = 1.0 / (length(uv - pos) * 16.0 + 1.0);
#if 1
    float temp2 = sin(ang * 2.0 + pos.x) * 4.0 - cos(ang * 3.0 + pos.y);
    //float temp3 = noise(float2(temp2, 0.0f));
    float temp3;
    {
        float2 p = float2(temp2, 0.0f);
        float2 ip = floor(p);
        float2 u = frac(p);
        u = u * u * (3.0 - 2.0 * u);
	
        float res = lerp(
		lerp(Random(ip), Random(ip + float2(1.0, 0.0)), u.x),
		lerp(Random(ip + float2(0.0, 1.0)), Random(ip + float2(1.0, 1.0)), u.x), u.y);
        temp3 = res * res;
    }
    float temp1 = temp3 * 16.0f;
    //float temp1 = temp2 * 16.0f;
    float temp0 = sin(temp1) * 0.1 + dist * 0.1 + 0.8;
    float temp = f0 * temp0;
    f0 = f0 + temp;
#else
	f0 = f0 + f0 * (sin(noise_map.Sample(sampler_states[LINEAR], float2(sin(ang * 2. + pos.x) * 4.0 - cos(ang * 3. + pos.y), 0.0)).x * 16.) * .1 + dist * .1 + .8);
#endif

	// Lens flare only 
    float f1 = max(0.01 - pow(length(uv + 1.2 * pos), 1.9), .0) * 7.0;

    float f2 = max(1.0 / (1.0 + 32.0 * pow(length(uvd + 0.8 * pos), 2.0)), .0) * 0.25;
    float f22 = max(1.0 / (1.0 + 32.0 * pow(length(uvd + 0.85 * pos), 2.0)), .0) * 0.23;
    float f23 = max(1.0 / (1.0 + 32.0 * pow(length(uvd + 0.9 * pos), 2.0)), .0) * 0.21;

    float2 uvx = lerp(uv, uvd, -0.5);
	 
    float f4 = max(0.01 - pow(length(uvx + 0.4 * pos), 2.4), .0) * 6.0;
    float f42 = max(0.01 - pow(length(uvx + 0.45 * pos), 2.4), .0) * 5.0;
    float f43 = max(0.01 - pow(length(uvx + 0.5 * pos), 2.4), .0) * 3.0;

    uvx = lerp(uv, uvd, -.4);

    float f5 = max(0.01 - pow(length(uvx + 0.2 * pos), 5.5), .0) * 2.0;
    float f52 = max(0.01 - pow(length(uvx + 0.4 * pos), 5.5), .0) * 2.0;
    float f53 = max(0.01 - pow(length(uvx + 0.6 * pos), 5.5), .0) * 2.0;

    uvx = lerp(uv, uvd, -0.5);

    float f6 = max(0.01 - pow(length(uvx - 0.3 * pos), 1.6), .0) * 6.0;
    float f62 = max(0.01 - pow(length(uvx - 0.325 * pos), 1.6), .0) * 3.0;
    float f63 = max(0.01 - pow(length(uvx - 0.35 * pos), 1.6), .0) * 5.0;

    float3 c = 0;

    c.r += f2 + f4 + f5 + f6;
    c.g += f22 + f42 + f52 + f62;
    c.b += f23 + f43 + f53 + f63;
    c = max(0, c * 1.3 - (length(uvd) * .05));

    return f0 * glory_light_intensity + c * lens_flare_intensity;
#else
	// Lens flare only 
	float intensity = 1.5;
	float2 main = uv - pos;
	float2 uvd = uv * (length(uv));

	float dist = length(main); 
	dist = pow(dist, .1);

	float f1 = max(0.01/*max radius*/ - pow(length(uv + 1.2/*distance*/ * pos), 1.9/*boost*/), .0) * 7.0;

	float f2 = max(1.0 / (1.0 + 32.0 * pow(length(uvd + 0.8 * pos), 2.0)), .0) * 00.1;
	float f22 = max(1.0 / (1.0 + 32.0 * pow(length(uvd + 0.85 * pos), 2.0)), .0) * 00.08;
	float f23 = max(1.0 / (1.0 + 32.0 * pow(length(uvd + 0.9 * pos), 2.0)), .0) * 00.06;

	float2 uvx = lerp(uv, uvd, -0.5);

	float f4 = max(0.01 - pow(length(uvx + 0.4 * pos), 2.4), .0) * 6.0;
	float f42 = max(0.01 - pow(length(uvx + 0.45 * pos), 2.4), .0) * 5.0;
	float f43 = max(0.01 - pow(length(uvx + 0.5 * pos), 2.4), .0) * 3.0;

	uvx = lerp(uv, uvd, -.4);

	float f5 = max(0.01 - pow(length(uvx + 0.2 * pos), 5.5), .0) * 2.0;
	float f52 = max(0.01 - pow(length(uvx + 0.4 * pos), 5.5), .0) * 2.0;
	float f53 = max(0.01 - pow(length(uvx + 0.6 * pos), 5.5), .0) * 2.0;

	uvx = lerp(uv, uvd, -0.5);

	float f6 = max(0.01 - pow(length(uvx - 0.3 * pos), 1.6), .0) * 6.0;
	float f62 = max(0.01 - pow(length(uvx - 0.325 * pos), 1.6), .0) * 3.0;
	float f63 = max(0.01 - pow(length(uvx - 0.35 * pos), 1.6), .0) * 5.0;

	float3 c = .0;

	c.r += f2 + f4 + f5 + f6; 
	c.g += f22 + f42 + f52 + f62; 
	c.b += f23 + f43 + f53 + f63;
	c = c * 1.3 - (length(uvd) * .05);

	return c * intensity;
#endif 
}

Texture2D texture0 : register(t0);
Texture2D depthTexture : register(t1);
#include "../../Define/SamplerStateDefine.hlsli"
SamplerState samplerStates[_SAMPLER_STATE_MAX] : register(s0);

float4 main(VsOut pin) : SV_TARGET
{
    float4 color = texture0.Sample(samplerStates[_LINEAR_WRAP_SAMPLER_INDEX], pin.texcoord);
    
    // カラーフィルター
#if 1
	// RGB > HSVに変換
    color.rgb = RGB2HSV(color.rgb);

	// 色相調整
    color.r += hueShift;

	// 彩度調整
    color.g *= saturation;

	// 明度調整
    color.b *= brightness;

	// HSV > RGBに変換
    color.rgb = HSV2RGB(color.rgb);
#endif
        
    // トーンマッピング
#if 1
    if (toneType == 0)
    {
        color.rgb *= 1.0 - exp(-color.rgb * exposure);
    }
    else if (toneType == 1)
    {
        color *= exp2(exposure);
        color /= 1.0 + color;
    }
    else if(toneType == 2)
    {
        color *= exp2(exposure);
        color = max(color - 0.004, 0.0);
        color = (color * (6.2 * color + 0.5)) / (color * (6.2 * color + 1.7) + 0.06);
    }
#endif
    
	// Lens flare
    float4 ndc_sun_position = mul(float4(-normalize(lightDirection.xyz) * sumDistance, 1), viewProjection);
    ndc_sun_position /= ndc_sun_position.w;
    [branch]
    if (saturate(ndc_sun_position.z) == ndc_sun_position.z)
    {
        float4 ndc_position;
	    //texture space to ndc
        ndc_position.x = pin.texcoord.x * +2 - 1;
        ndc_position.y = pin.texcoord.y * -2 + 1;
        ndc_position.z = depthTexture.Sample(samplerStates[_POINT_WRAP_SAMPLER_INDEX], pin.texcoord).x;
        ndc_position.w = 1;

	    //ndc to world space
        float4 world_position = mul(ndc_position, invViewProjection);
        world_position = world_position / world_position.w;

        float4 occluder;
        occluder.xy = ndc_sun_position.xy;
        //occluder.z = depthTexture.Sample(sampler_states[LINEAR_BORDER_BLACK], float2(ndc_sun_position.x * 0.5 + 0.5, 0.5 - ndc_sun_position.y * 0.5)).x;
        occluder.z = depthTexture.Sample(samplerStates[_LINEAR_BORDER_BLACK_INDEX], float2(ndc_sun_position.x * 0.5 + 0.5, 0.5 - ndc_sun_position.y * 0.5)).x;
        occluder = mul(float4(occluder.xyz, 1), invProjection);
        occluder /= occluder.w;
        float occluded_factor = step(250.0, occluder.z);

		//const float2 aspect_correct = float2(1.0, aspect);
        const float2 aspect_correct = float2(1.0 / aspect, 1.0);

        float sun_highlight_factor = max(0, dot(normalize(mul(world_position - cameraPosition, view)).xyz, float3(0, 0, 1)));
        float3 lens_flare_color = float3(1.4, 1.2, 1.0) * lens_flare(ndc_position.xy * aspect_correct, ndc_sun_position.xy * aspect_correct);
        //lens_flare_color -= noise(ndc_position.xy * 256) * .015;
        {
            float2 p = float2(ndc_position.xy * 256);
            float2 ip = floor(p);
            float2 u = frac(p);
            u = u * u * (3.0 - 2.0 * u);
	
            float res = lerp(
		    lerp(Random(ip), Random(ip + float2(1.0, 0.0)), u.x),
		    lerp(Random(ip + float2(0.0, 1.0)), Random(ip + float2(1.0, 1.0)), u.x), u.y);
            lens_flare_color -= res * res * .015;
        }
        lens_flare_color = cc(lens_flare_color, .5, .1);
        color.rgb += max(0.0, lens_flare_color) * occluded_factor * lightColor.rgb * 0.5 /* * directional_light_color[0].w*/;
    }
  //  float4 ndc_sun_position = mul(float4(-normalize(directional_light_direction[0].xyz) * distance_to_sun, 1), view_projection);
  //  ndc_sun_position /= ndc_sun_position.w;
  //  if (saturate(ndc_sun_position.z) == ndc_sun_position.z)
  //  {
  //      float4 occluder;
  //      occluder.xy = ndc_sun_position.xy;
  //      occluder.z = texture_maps[SCENE_DEPTH].Sample(sampler_states[LINEAR_BORDER_BLACK], float2(ndc_sun_position.x * 0.5 + 0.5, 0.5 - ndc_sun_position.y * 0.5)).x;
  //      occluder = mul(float4(occluder.xyz, 1), inverse_projection);
  //      occluder /= occluder.w;
  //      float occluded_factor = step(250.0, occluder.z);

		////const float2 aspect_correct = float2(1.0, aspect);
  //      const float2 aspect_correct = float2(1.0 / aspect, 1.0);

  //      float sun_highlight_factor = max(0, dot(normalize(mul(world_position - camera_position, view)).xyz, float3(0, 0, 1)));
  //      float3 lens_flare_color = float3(1.4, 1.2, 1.0) * lens_flare(ndc_position.xy * aspect_correct, ndc_sun_position.xy * aspect_correct);
  //      lens_flare_color -= noise(ndc_position.xy * 256) * .015;
  //      lens_flare_color = cc(lens_flare_color, .5, .1);
  //      fragment_color += max(0.0, lens_flare_color) * occluded_factor * directional_light_color[0].rgb * 0.5 /* * directional_light_color[0].w*/;
  //  }
    
#if 1
	// Gamma process
    const float GAMMA = 2.2;
    color = pow(color, 1.0 / GAMMA);
#endif
    
    // ヴィネット
#if 1
    float2 scene_map_size;
    texture0.GetDimensions(scene_map_size.x, scene_map_size.y);
    // 周辺減光処理
    float2 d = abs(pin.texcoord - vignetteCenter) * (vignetteIntensity);
    // 減光をスクリーンに合わすかどうか
    d.x *= lerp(1.0f, scene_map_size.x / scene_map_size.y, (float) vignetteRounded);
    // 隅の濃さ
    d = pow(saturate(d), vignetteRoundness);
    half vignette_factor = pow(saturate(1.0f - dot(d, d)), vignetteSmoothness);
    color.rgb *= lerp(vignetteColor.rgb, (float3) 1.0f, vignette_factor);
#endif
    
    return color;
}