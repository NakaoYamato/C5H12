#ifndef __INSTANCING_MODEL_CONSTANT_BUFFER_H__
#define __INSTANCING_MODEL_CONSTANT_BUFFER_H__

// スケルトン用定数バッファ
static const int INSTANCING_MAX = 100;
cbuffer CbSkeleton : register(b1)
{
    float4 materialColor[INSTANCING_MAX];
    row_major float4x4 worldTransform[INSTANCING_MAX];
}

#endif  //  __INSTANCING_MODEL_CONSTANT_BUFFER_H__