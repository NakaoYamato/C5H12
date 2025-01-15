#include "CascadedShadow.hlsli"

// スケルトン用定数バッファ
static const int INSTANCED_MAX = 100;
cbuffer CbSkeleton : register(b1)
{
    float4 materialColor;
    row_major float4x4 worldTransform[INSTANCED_MAX];
}

VS_OUT_CSM main(VS_IN vin, uint instance_id : SV_INSTANCEID)
{
    VS_OUT_CSM vout = (VS_OUT_CSM) 0;
    
    // SV_INSTANCEIDをインスタンシング描画とカスケードシャドウマップ描画でつかうので
    // ここで調整
    // 小数点切り捨てで処理
    uint modelInstanceId = instance_id / CASCADED_SHADOW_MAPS_SIZE;
    vout.instance_id = instance_id - (CASCADED_SHADOW_MAPS_SIZE * modelInstanceId);
    
    float4 worldPosition = mul(vin.position, worldTransform[modelInstanceId]);    
    vout.position = mul(float4(worldPosition.xyz, 1), csm_data.cascaded_matrices[vout.instance_id]);
    
    return vout;
}