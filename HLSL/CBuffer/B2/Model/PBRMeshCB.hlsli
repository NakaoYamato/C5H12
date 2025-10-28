#ifndef __PBR_MESH_CONSTANT_BUFFER_H__
#define __PBR_MESH_CONSTANT_BUFFER_H__

// メッシュ定数バッファ
cbuffer MESH_CONSTANT_BUFFER : register(b2)
{
    float4 meshBaseColor;
    
    float roughnessFactor;
    float metalnessFactor;
    int decalMask;
    float dummy;
}

#endif  //  __PBR_MESH_CONSTANT_BUFFER_H__