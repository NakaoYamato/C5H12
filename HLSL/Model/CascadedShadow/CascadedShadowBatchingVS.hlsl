#include "CascadedShadow.hlsli"
// スケルトン用定数バッファ
cbuffer CbSkeleton : register(b1)
{
    float4 materialColor;
    row_major float4x4 worldTransform;
}

VS_OUT_CSM main(VS_IN vin, uint instance_id : SV_INSTANCEID)
{
    VS_OUT_CSM vout = (VS_OUT_CSM) 0;
    
    float4 worldPosition = mul(vin.position, worldTransform);
    
    vout.instance_id = instance_id;
    vout.position = mul(float4(worldPosition.xyz, 1), csm_data.cascaded_matrices[instance_id]);
    
    return vout;
}