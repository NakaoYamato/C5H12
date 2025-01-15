#include "../ModelInputLayout.hlsli"

// カスケードシャドウマップの書き込む先の数
#define CASCADED_SHADOW_MAPS_SIZE 3

// CASCADED_SHADOW_MAPS
struct VS_OUT_CSM
{
    float4 position : SV_POSITION;
    uint instance_id : INSTANCEID;
};
struct GS_OUTPUT_CSM
{
    float4 position : SV_POSITION;
    uint render_target_array_index : SV_RENDERTARGETARRAYINDEX;
};

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
