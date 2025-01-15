#include "../ModelInputLayout.hlsli"
#define POINT 0
#define LINEAR 1
#define ANISOTROPIC 2
SamplerState sampler_states[3] : register(s0);
Texture2D texture_maps[4] : register(t0);

float4 main(VS_OUT pin) : SV_TARGET
{
    float4 diffuse_color = texture_maps[0].Sample(sampler_states[ANISOTROPIC], pin.texcoord);
    return diffuse_color * pin.materialColor;
}