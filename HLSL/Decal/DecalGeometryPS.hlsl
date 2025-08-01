#include "DecalGeometry.hlsli"
#include "../Define/SamplerStateDefine.hlsli"
SamplerState samplerStates[_SAMPLER_STATE_MAX] : register(s0);

//  デカールテクスチャ
Texture2D<float4> decaleColorTexture : register(t0);
Texture2D<float4> decaleNormalTexture : register(t1);

Texture2D<float4> gbufferColor : register(t2);
Texture2D<float> gbufferDepth : register(t3);

PS_OUT main(VS_OUT pin)
{
    float2 decalTexcoord = GetDecalTexcoord(pin.position.xy,
    gbufferDepth,
    samplerStates[_POINT_WRAP_SAMPLER_INDEX],
    invViewProjection,
    invWorld);
    
    float4 color = decaleColorTexture.Sample(samplerStates[_LINEAR_BORDER_BLACK_INDEX], decalTexcoord) * pin.color;
    clip(color.a < 0.5f ? -1 : 1);
    
    float4 normal = decaleNormalTexture.Sample(samplerStates[_LINEAR_BORDER_BLACK_INDEX], decalTexcoord);
    normal = normalize(mul(float4((normal * 2.0f - 1.0f).xyz, 0.0f), world));
    
    PS_OUT pout;
    pout.baseColor = color;
    pout.baseColor.a = 0.5f;
    pout.worldNormal = float4(normal.xyz, 0.5f);
    return pout;
}