#include "../ModelInputLayout.hlsli"

#include "../../CBuffer/B3/CascadedShadowCB.hlsli"

// カスケードシャドウマップの書き込む先の数
#define CASCADED_SHADOW_MAPS_SIZE 4

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
