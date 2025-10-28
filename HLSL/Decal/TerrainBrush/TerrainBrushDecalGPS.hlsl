#include "../DecalGeometry.hlsli"
#include "../../Define/SamplerStateDefine.hlsli"
SamplerState samplerStates[_SAMPLER_STATE_MAX] : register(s0);

#include "../DecalTextureResource.hlsli"

PS_OUT main(VS_OUT pin)
{
    DecalDecodeData data = GetDecalTexcoord(pin.position.xy,
    gbufferDepthMap,
    samplerStates[_POINT_WRAP_SAMPLER_INDEX],
    invViewProjection,
    invWorld);
    
    float4 gbufferColor = gbufferColorMap.Sample(samplerStates[_POINT_WRAP_SAMPLER_INDEX], data.gbufferTexcoord);
    
    float4 color = decaleColorTexture.Sample(samplerStates[_LINEAR_BORDER_BLACK_INDEX], data.cubeTexcoord);
    // colorの明るさに合わせてアルファ値を設定
    color.a = saturate(pow(length(color.rgb), 2.0f));
    clip(color.a < 0.1f ? -1 : 1);
    color.rgb = (color.rgb * pin.color.rgb) * color.a + gbufferColor.rgb * (1.0f - color.a);
    
    float4 normal = decaleNormalTexture.Sample(samplerStates[_LINEAR_BORDER_BLACK_INDEX], data.cubeTexcoord);
    normal = normalize(mul(float4((normal * 2.0f - 1.0f).xyz, 0.0f), world));
    
    PS_OUT pout;
    pout.baseColor = color;
    pout.baseColor.a = 0.5f;
    pout.worldNormal = float4(normal.xyz, 0.5f);
    return pout;
}