#include "TerrainShadow.hlsli"

// í∏ì_èÓïÒ
StructuredBuffer<StreamOutData> streamOutDatas : register(t2);

VS_OUT_CSM main(uint vertexId : SV_VertexID, uint instanceId : SV_INSTANCEID)
{
    VS_OUT_CSM vout = (VS_OUT_CSM) 0;
    
    vout.instanceId = instanceId;
    vout.position = mul(float4(streamOutDatas[vertexId].worldPosition.xyz, 1.0f), csm_data.cascaded_matrices[instanceId]);
    return vout;
}