#ifndef __INSTANCING_MODEL_CONSTANT_BUFFER_H__
#define __INSTANCING_MODEL_CONSTANT_BUFFER_H__

struct InstancingModelData
{
    row_major float4x4 worldTransform;
    float4 materialColor;
};

StructuredBuffer<InstancingModelData> matrixBuffer : register(t10);

#endif  //  __INSTANCING_MODEL_CONSTANT_BUFFER_H__