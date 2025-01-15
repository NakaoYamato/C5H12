#include "../ModelInputLayout.hlsli"

// スケルトン用定数バッファ
static const int INSTANCED_MAX = 100;
cbuffer CbSkeleton : register(b1)
{
    float4 materialColor;
    row_major float4x4 worldTransform[INSTANCED_MAX];
}

VS_OUT main(VS_IN vin, uint instance_id : SV_INSTANCEID)
{
    VS_OUT vout = (VS_OUT) 0;
    vout.world_position = mul(vin.position, worldTransform[instance_id]);
    
    vout.position = mul(vout.world_position, view_projection);
	
    vout.texcoord = vin.texcoord;
    
    vout.materialColor = materialColor;
    
    return vout;
}