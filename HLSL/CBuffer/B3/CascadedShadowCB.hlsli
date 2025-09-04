#ifndef __CASCADED_SHADOW_CONSTANT_BUFFER_H__
#define __CASCADED_SHADOW_CONSTANT_BUFFER_H__

struct csm_constants
{
    row_major float4x4 cascaded_matrices[4];
    float4 cascaded_plane_distances;
}; 
// カスケードシャドウマップ用定数バッファ
cbuffer csm_constants : register(b3)
{
    csm_constants csm_data;
}

#endif  //  __CASCADED_SHADOW_CONSTANT_BUFFER_H__