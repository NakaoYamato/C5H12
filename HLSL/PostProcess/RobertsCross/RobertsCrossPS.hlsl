#include "../../Sprite/Sprite.hlsli"

cbuffer ROBERTS_CROSS_BUFFER : register(b1)
{
    int enable;
    uint lineWidth; // ï‚ê≥íl
    float2 dummy;
};

#include "../../Function/ShadingFunctions.hlsli"

Texture2D texture0 : register(t0);
Texture2D texture1 : register(t1);
#include "../../Define/SamplerStateDefine.hlsli"
SamplerState samplerStates[_SAMPLER_STATE_MAX] : register(s0);

float4 main(VsOut pin) : SV_TARGET
{
    float4 color = texture0.Sample(samplerStates[_LINEAR_WRAP_SAMPLER_INDEX], pin.texcoord);
    
    float edge_factor = 1.0;
    if (enable)
    {
		// https://en.wikipedia.org/wiki/Roberts_cross
        float3 x00 = color.rgb;
        float3 x11 = texture0.Sample(samplerStates[_LINEAR_WRAP_SAMPLER_INDEX], pin.texcoord, int2(1, 1)).rgb;
        float3 x10 = texture0.Sample(samplerStates[_LINEAR_WRAP_SAMPLER_INDEX], pin.texcoord, int2(1, 0)).rgb;
        float3 x01 = texture0.Sample(samplerStates[_LINEAR_WRAP_SAMPLER_INDEX], pin.texcoord, int2(0, 1)).rgb;

        float3 y00 = sqrt(x00);
        float3 y11 = sqrt(x11);
        float3 y10 = sqrt(x10);
        float3 y01 = sqrt(x01);

        float3 z = sqrt((y00 - y11) * (y00 - y11) + (y10 - y01) * (y10 - y01));
        edge_factor = step(max(z.x, max(z.y, z.z)), 0.1);
        
#if 1
        float d00 = texture1.Sample(samplerStates[_LINEAR_WRAP_SAMPLER_INDEX], pin.texcoord, int2(0, 0) * 2).r;
        float d11 = texture1.Sample(samplerStates[_LINEAR_WRAP_SAMPLER_INDEX], pin.texcoord, int2(1, 1) * 2).r;
        float d10 = texture1.Sample(samplerStates[_LINEAR_WRAP_SAMPLER_INDEX], pin.texcoord, int2(1, 0) * 2).r;
        float d01 = texture1.Sample(samplerStates[_LINEAR_WRAP_SAMPLER_INDEX], pin.texcoord, int2(0, 1) * 2).r;

        d00 = pow(d00, 5);
        d11 = pow(d11, 5);
        d10 = pow(d10, 5);
        d01 = pow(d01, 5);

        float d = sqrt((d00 - d11) * (d00 - d11) + (d10 - d01) * (d10 - d01));
        edge_factor *= step(d, 0.001);
#endif
    }
    
    color.rgb *= edge_factor;
    return color;
}