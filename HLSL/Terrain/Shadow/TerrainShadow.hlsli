#include "../Terrain.hlsli"
#include "../../CBuffer/B3/CascadedShadowCB.hlsli"

struct VS_OUT_CSM
{
    float4 position : SV_POSITION;
    uint instanceId : INSTANCEID;
};
struct GS_OUTPUT_CSM
{
    float4 position : SV_POSITION;
    uint renderTargetArrayIndex : SV_RENDERTARGETARRAYINDEX;
};