#ifndef __OBJECT_CONSTANT_BUFFER_H__
#define __OBJECT_CONSTANT_BUFFER_H__

struct ObjectData
{
    float3 position;
    float radius;
};

// オブジェクトの最大数
#define MaxObjectConstant 5

// オブジェクト定数バッファ
cbuffer OBJECT_CONSTANT_BUFFER : register(b5)
{
    ObjectData objectData[MaxObjectConstant];
}

#endif  //  __OBJECT_CONSTANT_BUFFER_H__