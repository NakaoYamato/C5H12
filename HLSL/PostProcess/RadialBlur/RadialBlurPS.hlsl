#include "../../Sprite/Sprite.hlsli"

cbuffer RADIAL_BLUR_BUFFER : register(b1)
{
    float blurRadius;
    int blurSamplingCount;
    float2 blurCenter; // UV Space

    float blurMaskRadius;
    float3 blurDummy;
};

Texture2D texture0 : register(t0);
#include "../../Define/SamplerStateDefine.hlsli"
SamplerState samplerStates[_SAMPLER_STATE_MAX] : register(s0);

float4 main(VsOut pin) : SV_TARGET
{
    float2 scene_map_size;
    texture0.GetDimensions(scene_map_size.x, scene_map_size.y);
    
    float4 color = texture0.Sample(samplerStates[_LINEAR_WRAP_SAMPLER_INDEX], pin.texcoord);
    float4 result_color = color;
    
    float2 blur_vector = (blurCenter - pin.texcoord);
    blur_vector *= (blurRadius / scene_map_size.xy) / blurSamplingCount;
    for (int index = 1; index < blurSamplingCount; ++index)
    {
        result_color += texture0.Sample(samplerStates[_LINEAR_WRAP_SAMPLER_INDEX], pin.texcoord + blur_vector * index);
    }

    // Žw’è‚Ì”ÍˆÍ“à‚Í“K‰ž—Ê‚ð•Ï‚¦‚é
    float mask_radius = blurMaskRadius / min(scene_map_size.x, scene_map_size.y);
    float mask_value = saturate(length(pin.texcoord - blurCenter) / mask_radius);
    return lerp(color, result_color / blurSamplingCount, mask_value);
}