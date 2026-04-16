#include "TerrainGrass.hlsli"

GRASS_HS_IN main(StreamOutData input)
{
    GRASS_HS_IN vout;
    vout.position = input.position;
    vout.worldPosition = input.worldPosition;
    vout.worldNormal = input.worldNormal;
    vout.worldTangent = input.worldTangent;
    vout.parameter = input.parameter;
    vout.texcoord = input.texcoord;
    
    return vout;
}