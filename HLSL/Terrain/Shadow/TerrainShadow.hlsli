#include "../Terrain.hlsli"
#include "../../CBuffer/B3/CascadedShadowCB.hlsli"

struct CSM_VS_IN
{
    float3 position : POSITION;
    uint instanceId : SV_INSTANCEID;
};

#define CSM_HS_IN CSM_VS_IN
#define CSM_HS_OUT CSM_HS_IN
#define CSM_DS_IN CSM_HS_IN

struct CSM_DS_OUT
{
    float4 position : SV_POSITION;
    uint instanceId : INSTANCEID;
};