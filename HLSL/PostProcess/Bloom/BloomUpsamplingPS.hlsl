#include "bloom.hlsli"

SamplerState linearSampler : register(s2);

static const uint downsampled_count = 6;
Texture2D downsampled_textures[downsampled_count] : register(t0);

float4 main(VsOut pin) : SV_TARGET
{
    float3 sampled_color = 0;
	[unroll]
    for (uint downsampled_index = 0; downsampled_index < downsampled_count; ++downsampled_index)
    {
        sampled_color += downsampled_textures[downsampled_index].Sample(linearSampler, pin.texcoord).xyz;
    }
    return float4(sampled_color * bloom_intensity, 1);
}
