#ifndef __PHONG_MESH_CONSTANT_BUFFER_H__
#define __PHONG_MESH_CONSTANT_BUFFER_H__

// メッシュ定数バッファ
cbuffer MESH_CONSTANT_BUFFER : register(b2)
{
    float4 Ka;
    float4 Kd;
    float4 Ks;
}

#endif  //  __PHONG_MESH_CONSTANT_BUFFER_H__