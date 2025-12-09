#include "../Terrain.hlsli"
#include "../../CBuffer/B3/CascadedShadowCB.hlsli"

struct VS_OUT_CSM
{
    float3 position : POSITION;
    uint instanceId : INSTANCEID;
};

// HS入力(VS出力と同じ)
#define HS_IN_CSM VS_OUT_CSM

// HS出力(VS出力と同じ)
#define HS_OUT_CSM HS_IN_CSM
// DS入力(VS出力と同じ)
#define DS_IN_CSM HS_IN_CSM

struct DS_OUT_CSM
{
    float4 position : SV_POSITION;
    uint instanceId : INSTANCEID;
};

struct GS_OUTPUT_CSM
{
    float4 position : SV_POSITION;
    uint renderTargetArrayIndex : SV_RENDERTARGETARRAYINDEX;
};