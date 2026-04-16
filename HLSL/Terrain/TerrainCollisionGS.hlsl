#include "Terrain.hlsli"

#include "../Define/SamplerStateDefine.hlsli"
SamplerState samplerStates[_SAMPLER_STATE_MAX] : register(s0);

[maxvertexcount(3)]
void main(
	triangle GS_IN gin[3] : SV_POSITION,
	inout TriangleStream<CollisionStreamOutData> gout
)
{
    for (uint i = 0; i < 3; i++)
    {
        CollisionStreamOutData elemant;
        elemant.worldPosition = gin[i].worldPosition;
        elemant.padding = 0.0f;
        
        gout.Append(elemant);
    }
    gout.RestartStrip();
}