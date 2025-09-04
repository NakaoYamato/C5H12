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
        elemant.worldPosition = gin[i].worldPosition;
        elemant.worldNormal = gin[i].worldNormal;
        elemant.worldTangent = gin[i].worldTangent;
        elemant.texcoord = gin[i].texcoord;
        // コストをデータマップから取得
        float4 parameter = parameterTexture.SampleLevel(samplerStates[_POINT_CLAMP_SAMPLER_INDEX], elemant.texcoord, 0);
        elemant.parameter = parameter;
        
        gout.Append(elemant);
    }
    gout.RestartStrip();
}