#include "Terrain.hlsli"

HS_IN main(float4 position : POSITION, float4 normal : NORMAL, float2 texcoord : TEXCOORD)
{
    HS_IN vout = (HS_IN) 0;
    vout.position = position;
    vout.normal = normal.xyz;
    // TODO 
    vout.color = float4(1, 1, 1, 1);
    vout.texcoord = texcoord;
    return vout;
}