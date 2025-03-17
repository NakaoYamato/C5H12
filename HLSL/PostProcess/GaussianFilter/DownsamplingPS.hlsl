#include "../../Sprite/Sprite.hlsli"

#include "../../Define/SamplerStateDefine.hlsli"
SamplerState samplerStates[_SAMPLER_STATE_MAX] : register(s0);

Texture2D textureMap : register(t0);

float4 main(VsOut pin) : SV_TARGET
{
    return textureMap.Sample(samplerStates[_LINEAR_WRAP_SAMPLER_INDEX], pin.texcoord, 0.0);
}
