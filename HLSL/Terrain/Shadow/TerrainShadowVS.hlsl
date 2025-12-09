#include "TerrainShadow.hlsli"

// í∏ì_èÓïÒ
StructuredBuffer<StreamOutData> streamOutDatas : register(t2);

VS_OUT_CSM main(VS_IN vin, uint instanceId : SV_INSTANCEID)
{
    VS_OUT_CSM vout = (VS_OUT_CSM) 0;
    
    vout.instanceId = instanceId;
    vout.position = vin.position;
    return vout;
}