#include "../../Sprite/Sprite.hlsli"

Texture2D<float4> texture0 : register(t0);
Texture2D<float4> texture1 : register(t1);
Texture2D<float4> texture2 : register(t2);

#include "../../Define/SamplerStateDefine.hlsli"
SamplerState samplerStates[_SAMPLER_STATE_MAX] : register(s0);

struct PS_OUT
{
    float4 color : SV_TARGET0;
    float4 normal : SV_TARGET1;
    float4 height : SV_TARGET2;
};

PS_OUT main(VsOut pin)
{
    PS_OUT pout = (PS_OUT) 0;
    pout.color = texture0.Sample(samplerStates[_POINT_WRAP_SAMPLER_INDEX], pin.texcoord);
    pout.normal = texture1.Sample(samplerStates[_POINT_WRAP_SAMPLER_INDEX], pin.texcoord);
    pout.height = texture2.Sample(samplerStates[_POINT_WRAP_SAMPLER_INDEX], pin.texcoord);
    return pout;
}