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
