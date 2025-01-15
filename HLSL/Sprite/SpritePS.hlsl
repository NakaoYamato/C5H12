#include "Sprite.hlsli"

Texture2D color_map : register(t0);
SamplerState point_sampler_state : register(s0);
SamplerState linear_sampler_state : register(s2);

float4 main(VsOut pin) : SV_TARGET
{
    float4 color = color_map.Sample(point_sampler_state, pin.texcoord) * pin.color;
    float alpha = color.a;

    return float4(color.rgb, alpha);
}