#include "Terrain.hlsli"

HS_IN main(VS_IN vin)
{
    HS_IN vout = (HS_IN) 0;
    vout.position = vin.position;
    vout.normal = vin.normal.xyz;
    vout.texcoord = vin.texcoord;
    return vout;
}