#include "../ModelInputLayout.hlsli"

// スケルトン用定数バッファ
cbuffer CbSkeleton : register(b1)
{
    float4 materialColor;
    row_major float4x4 worldTransform;
}

VS_OUT main(VS_IN vin)
{
    VS_OUT vout = (VS_OUT) 0;
    vout.world_position = mul(vin.position, worldTransform);
    
    vout.position = mul(vout.world_position, view_projection);
	
    vout.texcoord = vin.texcoord;
    
    vout.materialColor = materialColor;
    
    return vout;
}