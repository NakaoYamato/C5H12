#include "GaussianFilter.hlsli"

#include "../../Define/SamplerStateDefine.hlsli"
SamplerState samplerStates[_SAMPLER_STATE_MAX] : register(s0);

Texture2D downsampled_textures[DownsampledCount] : register(t0);

float4 main(VsOut pin) : SV_TARGET
{
    float3 sampled_color = 0;
	[unroll]
    for (uint downsampled_index = 0; downsampled_index < DownsampledCount; ++downsampled_index)
    {
        sampled_color += downsampled_textures[downsampled_index].Sample(samplerStates[_LINEAR_WRAP_SAMPLER_INDEX], pin.texcoord).xyz;
    }
    return float4(sampled_color * intensity, 1);
}
