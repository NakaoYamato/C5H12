#include "../../Sprite/Sprite.hlsli"

SamplerState linearSampler : register(s2);

Texture2D hdr_color_buffer_texture : register(t0);

float4 main(VsOut pin) : SV_TARGET
{
    return hdr_color_buffer_texture.Sample(linearSampler, pin.texcoord, 0.0);
}
