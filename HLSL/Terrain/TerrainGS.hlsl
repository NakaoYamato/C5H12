#include "Terrain.hlsli"

#include "../Define/SamplerStateDefine.hlsli"
SamplerState samplerStates[_SAMPLER_STATE_MAX] : register(s0);

[maxvertexcount(3)]
void main(
	triangle GS_IN gin[3] : SV_POSITION,
	inout TriangleStream<GS_OUT> gout
)
{
    for (uint i = 0; i < 3; i++)
    {
		GS_OUT elemant;
        elemant.position = gin[i].position;
        elemant.worldPosition = gin[i].worldPosition;
        elemant.worldNormal = gin[i].worldNormal;
        elemant.worldTangent = gin[i].worldTangent;
        elemant.texcoord = gin[i].texcoord;
        elemant.parameter = gin[i].parameter;
        
        gout.Append(elemant);
    }
    gout.RestartStrip();
}