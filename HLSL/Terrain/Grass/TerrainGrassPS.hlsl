#include "TerrainGrass.hlsli"
#include "../../Define/SamplerStateDefine.hlsli"
SamplerState samplerStates[_SAMPLER_STATE_MAX] : register(s0);

Texture2D colorMap : register(t4);

float4 main(GRASS_PS_IN pin) : SV_TARGET
{
    float4 color = colorMap.Sample(samplerStates[_ANISOTROPIC_SAMPLER_INDEX], pin.texcoord);
    const float3 grass_withered_color = saturate(color.rgb + pin.color.rgb);
    float3 diffuse_color = lerp(grass_withered_color, grass_withered_color * 0.2, pin.texcoord.y);
    return float4(diffuse_color, 1.0f);
}