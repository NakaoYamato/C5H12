#include "TerrainGrass.hlsli"

#include "../../GBuffer/GBuffer.hlsli"

#include "../../Define/SamplerStateDefine.hlsli"
SamplerState samplerStates[_SAMPLER_STATE_MAX] : register(s0);

Texture2D colorMap : register(t4);

PS_GB_OUT main(GRASS_PS_IN pin)
{
    float4 color = colorMap.Sample(samplerStates[_ANISOTROPIC_SAMPLER_INDEX], pin.texcoord);
    const float3 grass_withered_color = saturate(color.rgb + pin.color.rgb);
    float3 diffuse_color = lerp(grass_withered_color, grass_withered_color * 0.2, pin.texcoord.y);
    
    return CreateOutputData(
    diffuse_color.rgb,
    0.5f,
    pin.worldNormal,
    0.5f,
    float3(0.0f, 0.0f, 0.0f),
    0.5f);
}