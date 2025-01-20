#include "../../Sprite/Sprite.hlsli"
SamplerState pointSampler : register(s0);
Texture2D texture_maps[2] : register(t0);

float4 main(VsOut pin) : SV_TARGET
{
    float4 color = texture_maps[0].Sample(pointSampler, pin.texcoord);
    float4 bloom = texture_maps[1].Sample(pointSampler, pin.texcoord);

    float3 fragment_color = color.rgb + bloom.rgb;
    float alpha = color.a;
    
    return float4(fragment_color, alpha);
}