#include "CascadedShadow.hlsli"
#include "../Skinning.hlsli"

VS_OUT_CSM main(VS_IN vin, uint instance_id : SV_INSTANCEID)
{
    VS_OUT_CSM vout = (VS_OUT_CSM)0;
    
    // アニメーション処理
    float4 worldPosition = SkinningPosition(vin.position, vin.boneWeights, vin.boneIndices);
    
    vout.instance_id = instance_id;
    vout.position = mul(worldPosition, csm_data.cascaded_matrices[instance_id]);
    
    return vout;
}