#include "Sprite.hlsli"

Texture2D colorMap : register(t0);
#include "../Define/SamplerStateDefine.hlsli"
SamplerState samplerStates[_SAMPLER_STATE_MAX] : register(s0);

cbuffer CBScroll : register(b2)
{
    float scrollU;
    float scrollV;
    float2 padding;
};

float4 main(VsOut pin) : SV_TARGET
{
    float2 texcoord = pin.texcoord;
    texcoord.x += scrollU;
    texcoord.y += scrollV;
    
    float4 color = colorMap.Sample(samplerStates[_POINT_WRAP_SAMPLER_INDEX], texcoord) * pin.color;
    if (color.a < 0.01f)
        discard;

    return color;

}