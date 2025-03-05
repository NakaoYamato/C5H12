#include "Grass.hlsli"

// スケルトン用定数バッファ
cbuffer CbSkeleton : register(b1)
{
    float4 materialColor;
    row_major float4x4 worldTransform;
}

GRASS_VS_OUT main(VS_IN vin)
{
    GRASS_VS_OUT vout;
    //vout.position = vin.position;
    vout.world_position = mul(vin.position, worldTransform);
    return vout;
}