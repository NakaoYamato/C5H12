#include "Terrain.hlsli"

#include "../Define/SamplerStateDefine.hlsli"
SamplerState samplerStates[_SAMPLER_STATE_MAX] : register(s0);
// パラメータマップ
Texture2D<float4> parameterTexture : register(t6);

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
        elemant.texcoord = gin[i].texcoord;
        elemant.normal = gin[i].normal;
        elemant.worldPosition = gin[i].worldPosition;
        // コストをデータマップから取得
        elemant.cost = parameterTexture.SampleLevel(samplerStates[_POINT_CLAMP_SAMPLER_INDEX], elemant.texcoord, 0).g;
        
        gout.Append(elemant);
    }
    gout.RestartStrip();
}