#include "Terrain.hlsli"

#define STATIC_VS_OUT StreamOutData

// í∏ì_èÓïÒ
StructuredBuffer<StreamOutData> streamOutDatas : register(t2);

STATIC_VS_OUT main(uint vertexId : SV_VertexID)
{
	STATIC_VS_OUT vout = streamOutDatas[vertexId];
    vout.position = mul(float4(vout.worldPosition.xyz, 1.0f), viewProjection);
    return vout;
}