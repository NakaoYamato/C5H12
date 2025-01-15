#include "../../Sprite/Sprite.hlsli"

cbuffer VIGNETTE_BUFFER : register(b1)
{
    float4 vignetteColor;

    float2 vignetteCenter;
    float vignetteIntensity;
    float vignetteSmoothness;

    int vignetteRounded; // 1 : use  0 : unUse
    float vignetteRoundness;
    float vignetteDummy[2];
};

Texture2D texture0 : register(t0);
SamplerState sampler0 : register(s0);

float4 main(VsOut pin) : SV_TARGET
{
    float2 scene_map_size;
    texture0.GetDimensions(scene_map_size.x, scene_map_size.y);
    
    float4 color = texture0.Sample(sampler0, pin.texcoord);
    
    // ü•ÓŒ¸Œõˆ—
    float2 d = abs(pin.texcoord - vignetteCenter) * (vignetteIntensity);
    // Œ¸Œõ‚ğƒXƒNƒŠ[ƒ“‚É‡‚í‚·‚©‚Ç‚¤‚©
    d.x *= lerp(1.0f, scene_map_size.x / scene_map_size.y, (float)vignetteRounded);
    // ‹÷‚Ì”Z‚³
    d = pow(saturate(d), vignetteRoundness);
    half vignette_factor = pow(saturate(1.0f - dot(d, d)), vignetteSmoothness);
    color.rgb *= lerp(vignetteColor.rgb, (float3) 1.0f, vignette_factor);
    return color;
}