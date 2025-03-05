#include "Grass.hlsli"

GRASS_VS_OUT main(VS_IN vin)
{
    GRASS_VS_OUT vout;
    vout.position = vin.position;
    return vout;
}