#ifndef __BATCHING_MODEL_CONSTANT_BUFFER_H__
#define __BATCHING_MODEL_CONSTANT_BUFFER_H__

// スケルトン用定数バッファ
cbuffer CbSkeleton : register(b1)
{
    float4 materialColor;
    row_major float4x4 worldTransform;
}

#endif  //  __BATCHING_MODEL_CONSTANT_BUFFER_H__