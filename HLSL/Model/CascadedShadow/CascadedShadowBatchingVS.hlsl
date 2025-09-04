#include "CascadedShadow.hlsli"

#include "../../CBuffer/B1/Skeleton/BatchingModelCB.hlsli"

VS_OUT_CSM main(VS_IN vin, uint instance_id : SV_INSTANCEID)
{
    VS_OUT_CSM vout = (VS_OUT_CSM) 0;
    
    float4 worldPosition = mul(vin.position, worldTransform);
    
    vout.instance_id = instance_id;
    vout.position = mul(float4(worldPosition.xyz, 1), csm_data.cascaded_matrices[instance_id]);
    
    return vout;
}