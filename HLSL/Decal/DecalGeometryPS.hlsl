#include "DecalGeometry.hlsli"
#include "../Define/SamplerStateDefine.hlsli"
SamplerState samplerStates[_SAMPLER_STATE_MAX] : register(s0);

#include "DecalTextureResource.hlsli"

PS_OUT main(VS_OUT pin)
{
    DecalDecodeData data = GetDecalTexcoord(pin.position.xy,
    gbufferDepthMap,
    samplerStates[_POINT_WRAP_SAMPLER_INDEX],
    invViewProjection,
    invWorld);
    
    DecalMask(data.gbufferTexcoord, gbufferParameterMap, samplerStates[_LINEAR_BORDER_BLACK_INDEX], decalMask);
    
    float4 color = decaleColorTexture.Sample(samplerStates[_LINEAR_BORDER_BLACK_INDEX], data.cubeTexcoord) * pin.color;
    clip(color.a < 0.5f ? -1 : 1);
    
    float4 normal = decaleNormalTexture.Sample(samplerStates[_LINEAR_BORDER_BLACK_INDEX], data.cubeTexcoord);
    normal = normalize(mul(float4((normal * 2.0f - 1.0f).xyz, 0.0f), world));
    
    PS_OUT pout;
    pout.baseColor = color;
    pout.baseColor.a = 0.5f;
    pout.worldNormal = float4(normal.xyz, 0.5f);
    return pout;
}