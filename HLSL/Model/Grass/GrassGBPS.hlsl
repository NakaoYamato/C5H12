#include "Grass.hlsli"

#include "../../GBuffer/GBuffer.hlsli"

PS_GB_OUT main(GS_OUT pin)
{
    float3 L = directional_light_direction.xyz;
    float3 N = normalize(pin.normal.xyz);
    float diffuse_factor = max(0, dot(N, L) * 0.5 + 0.5);
    const float3 tip_color = float3(0, 1, 0);
    const float3 root_color = float3(0.2, 0.1, 0);
    float4 diffuseColor = float4(lerp(tip_color, root_color, pin.texcoord.y) * diffuse_factor, 1);

    return CreateOutputData(diffuseColor, 0.0f, pin.position, float4(N, 1.0f));
}
