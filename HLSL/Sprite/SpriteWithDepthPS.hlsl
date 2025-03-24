#include "../Sprite/Sprite.hlsli"

Texture2D colorMap : register(t0);
Texture2D depthMap : register(t1);
#include "../Define/SamplerStateDefine.hlsli"
SamplerState samplerStates[_SAMPLER_STATE_MAX] : register(s0);

//  ê[ìxílÇèoóÕÇ∑ÇÈ
struct PS_OUT
{
    float4 color : SV_TARGET;
    float depth : SV_DEPTH;
};

PS_OUT main(VsOut pin)
{
    PS_OUT pout;
    pout.color = colorMap.Sample(samplerStates[_POINT_WRAP_SAMPLER_INDEX], pin.texcoord) * pin.color;
    pout.depth = depthMap.Sample(samplerStates[_POINT_WRAP_SAMPLER_INDEX], pin.texcoord).r;
    return pout;
}