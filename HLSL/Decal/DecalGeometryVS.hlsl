#include "DecalGeometry.hlsli"

VS_OUT main(float4 position : POSITION, float4 normal : NORMAL, float2 texcoord : TEXCOORD)
{
    VS_OUT vout;
    vout.position = mul(position, mul(world, viewProjection));
    vout.color = material_color;
    vout.texcoord = texcoord;
    return vout;
}
