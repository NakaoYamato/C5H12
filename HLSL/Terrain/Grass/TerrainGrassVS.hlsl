#include "TerrainGrass.hlsli"

// ’¸“_î•ñ
StructuredBuffer<StreamOutData> streamOutDatas : register(t0);

GRASS_HS_IN main(uint vertexId : SV_VertexID)
{
    // ’¸“_”Ô†‚ğ‘—‚é‚¾‚¯
    GRASS_HS_IN vout = streamOutDatas[vertexId];
    return vout;
}