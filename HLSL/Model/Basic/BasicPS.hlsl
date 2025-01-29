#include "../ModelInputLayout.hlsli"
#define POINT 0
#define LINEAR 1
#define ANISOTROPIC 2
SamplerState sampler_states[3] : register(s0);
Texture2D texture_maps[4] : register(t0);

// メッシュ定数バッファ
cbuffer MESH_CONSTANT_BUFFER : register(b2)
{
    float4 diffuse;
}

float4 main(VS_OUT pin) : SV_TARGET
{
    float4 diffuse_color = texture_maps[0].Sample(sampler_states[ANISOTROPIC], pin.texcoord);
    float3 color = diffuse_color.rgb * pin.materialColor.rgb * diffuse.rgb;
    float alpha = diffuse_color.a * pin.materialColor.a * diffuse.a;
    return float4(color, alpha);
}