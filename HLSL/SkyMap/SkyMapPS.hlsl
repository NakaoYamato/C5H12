#include "SkyMap.hlsli"

#include "../Define/SamplerStateDefine.hlsli"
SamplerState samplerStates[_SAMPLER_STATE_MAX] : register(s0);
Texture2D skymap : register(t0); // latitude-longitude mapped texture

float4 main(VsOut pin) : SV_TARGET
{
    float4 R = mul(float4((pin.texcoord.x * 2.0) - 1.0, 1.0 - (pin.texcoord.y * 2.0), 1, 1), inverse_view_projection);
    R /= R.w;
    float3 v = normalize(R.xyz);

	// Blinn/Newell Latitude Mapping
    const float PI = 3.14159265358979;
    float2 sample_point;
    sample_point.x = (atan2(v.z, v.x) + PI) / (PI * 2.0);
    sample_point.y = 1.0 - ((asin(v.y) + PI * 0.5) / PI);

    const float lod = 0;
    return skymap.SampleLevel(samplerStates[_LINEAR_WRAP_SAMPLER_INDEX], sample_point, lod);
}
