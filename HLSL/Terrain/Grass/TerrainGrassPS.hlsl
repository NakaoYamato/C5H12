#include "TerrainGrass.hlsli"
#include "../../Define/SamplerStateDefine.hlsli"
SamplerState samplerStates[_SAMPLER_STATE_MAX] : register(s0);

Texture2D colorMap : register(t4);

float4 main(GRASS_PS_IN pin) : SV_TARGET
{
    float3 color = colorMap.Sample(samplerStates[_ANISOTROPIC_SAMPLER_INDEX], pin.texcoord).rgb * pin.color.rgb;
    
    return float4(color.rgb, 1.0f);
}