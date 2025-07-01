#include "TerrainDeform.hlsli"

float4 main(VsOut pin) : SV_TARGET
{
    // ƒuƒ‰ƒV‚Ì‰e‹¿“x‚ğó‚¯‚é‚©”»’è
    float len = length(pin.texcoord - brushUVPosition);
    clip(brushRadius - len);

    // ‰e‹¿Š„‡‚ğŒvZ
    float rate = 1.0f - saturate(len / brushRadius);
    
    float4 baseColor = texture0.SampleLevel(samplerStates[_POINT_WRAP_SAMPLER_INDEX], pin.texcoord, 0);
    baseColor.a = clamp(baseColor.a + brushStrength * rate, heightScale.x, heightScale.y);
    baseColor.rgb = baseColor.rgb;
    return baseColor;
}