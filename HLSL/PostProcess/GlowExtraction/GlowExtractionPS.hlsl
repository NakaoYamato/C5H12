#include "../../Sprite/Sprite.hlsli"

cbuffer GLOW_EXTRACTION_CONSTANT_BUFFER : register(b1)
{
    float extractionThreshold;
    float3 glowDummy;
};

#include "../../Define/SamplerStateDefine.hlsli"
SamplerState samplerStates[_SAMPLER_STATE_MAX] : register(s0);

Texture2D hdrTexture : register(t0);
float4 main(VsOut pin) : SV_TARGET
{
    float4 sampled_color = hdrTexture.Sample(samplerStates[_LINEAR_WRAP_SAMPLER_INDEX], pin.texcoord);

#if 0
	return float4(step(extractionThreshold, dot(sampled_color.rgb, float3(0.299, 0.587, 0.114))) * sampled_color.rgb * bloom_intensity, sampled_color.a);
#else	
    return float4(step(extractionThreshold, max(sampled_color.r, max(sampled_color.g, sampled_color.b))) * sampled_color.rgb, sampled_color.a);
#endif	
}
