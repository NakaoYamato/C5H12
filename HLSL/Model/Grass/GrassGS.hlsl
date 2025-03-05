#include "Grass.hlsli"

// GRASS.02
// https://thebookofshaders.com/11/?lan=jp
// 2D Random
float random(in float2 st)
{
    return frac(sin(dot(st.xy, float2(12.9898, 78.233))) * 43758.5453123);
}
// 2D Noise based on Morgan McGuire @morgan3d
// https://www.shadertoy.com/view/4dS3Wd
float noise(in float2 st)
{
    float2 i = floor(st);
    float2 f = frac(st);

	// Four corners in 2D of a tile
    float a = random(i);
    float b = random(i + float2(1.0, 0.0));
    float c = random(i + float2(0.0, 1.0));
    float d = random(i + float2(1.0, 1.0));

	// Smooth Interpolation

	// Cubic Hermine Curve.  Same as SmoothStep()
    float2 u = f * f * (3.0 - 2.0 * f);

	// Mix 4 coorners percentages
    return lerp(a, b, u.x) + (c - a) * u.y * (1.0 - u.x) + (d - b) * u.x * u.y;
}
float4x4 angle_axis(float angle, float3 axis)
{
    float c, s;
    sincos(angle, s, c);

    float t = 1 - c;
    float x = axis.x;
    float y = axis.y;

    float z = axis.z;
    return float4x4(
		t * x * x + c, t * x * y - s * z, t * x * z + s * y, 0,
		t * x * y + s * z, t * y * y + c, t * y * z - s * x, 0,
		t * x * z - s * y, t * y * z + s * x, t * z * z + c, 0,
		0, 0, 0, 1
		);
}

// スケルトン用定数バッファ
cbuffer CbSkeleton : register(b1)
{
    float4 materialColor;
    row_major float4x4 worldTransform;
}

[maxvertexcount(3)]
void main(triangle DS_OUT input[3], inout TriangleStream<GS_OUT> output)
{
    float4 midpoint_position = (input[0].world_position + input[1].world_position + input[2].world_position) / 3;
    float4 midpoint_normal = float4(normalize(cross(input[1].world_position.xyz - input[0].world_position.xyz, input[2].world_position.xyz - input[0].world_position.xyz)), 0);
    float4 midpoint_tangent = float4(1, 0, 0, 0);
    
	// GRASS.02
    const float noise_factor = noise(midpoint_position.xz * noise_seed_multiplier);
    float height = lerp(grass_blade_height * 0.2, grass_blade_height, noise_factor); // GRASS.02
    float width = lerp(grass_blade_width * 0.2, grass_blade_width, noise_factor); // GRASS.02
    midpoint_position.xz += (noise_factor * 2.0 - 1.0) * 0.5;
    float4x4 R = angle_axis(noise_factor * 3.14159265358979, midpoint_normal.xyz);
    midpoint_tangent = mul(midpoint_tangent, R);
	


    GS_OUT element;
    element.normal = midpoint_normal;

    element.position = midpoint_position + midpoint_normal * height;
    element.sv_position = mul(element.position, view_projection);
    element.texcoord = float2(0.5, 0.0);
    output.Append(element);

    element.position = midpoint_position + midpoint_tangent * width;
    element.sv_position = mul(element.position, view_projection);
    element.texcoord = float2(0.0, 1.0);
    output.Append(element);

    element.position = midpoint_position - midpoint_tangent * width;
    element.sv_position = mul(element.position, view_projection);
    element.texcoord = float2(1.0, 1.0);
    output.Append(element);

    output.RestartStrip();
}