#include "TerrainDeform.hlsli"

float4 main(VsOut pin) : SV_TARGET
{
    // ƒuƒ‰ƒV‚Ì‰e‹¿“x‚ğó‚¯‚é‚©”»’è
    float len = length(pin.texcoord - brushUVPosition);
    clip(brushRadius - len);

    // ‰e‹¿Š„‡‚ğŒvZ
    float rate = 1.0f - saturate(len / brushRadius);

    float3 color = brushColor.rgb;
    float4 baseColor = texture0.Sample(samplerStates[_POINT_WRAP_SAMPLER_INDEX], pin.texcoord);
    color = baseColor.rgb - color * brushStrength * rate;
    color = saturate(color);
    return float4(color, baseColor.a);
}