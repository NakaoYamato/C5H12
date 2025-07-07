#include "../ModelInputLayout.hlsli"

#include "../../GBuffer/GBuffer.hlsli"

PS_GB_OUT main(VS_OUT pin)
{
	// ƒx[ƒX‚ğ”’‚Åˆ—
    float4 baseColor = float4(1.0f, 1.0f, 1.0f, 1.0f);
    
    float3 N = normalize(pin.world_normal.xyz);
    float roughness = 0.5f;
    float metalness = 0.5f;
    
    return CreateOutputData(
    baseColor.rgb,
    0.0f,
    N,
    metalness,
    float3(0.0f, 0.0f, 0.0f),
    roughness);
}