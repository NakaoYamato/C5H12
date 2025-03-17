#include "SkyMap.hlsli"

#include "../Define/SamplerStateDefine.hlsli"
SamplerState samplerStates[_SAMPLER_STATE_MAX] : register(s0);
TextureCube skybox : register(t0); // latitude-longitude mapped texture

float4 main(VsOut pin) : SV_TARGET
{
    float4 R = mul(float4((pin.texcoord.x * 2.0) - 1.0, 1.0 - (pin.texcoord.y * 2.0), 1.0, 1.0), inverse_view_projection);
    R /= R.w;

    const float lod = 0;
    return skybox.SampleLevel(samplerStates[_LINEAR_WRAP_SAMPLER_INDEX], R.xyz, lod);
}
