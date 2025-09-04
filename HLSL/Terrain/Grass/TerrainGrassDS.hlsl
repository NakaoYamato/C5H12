#include "TerrainGrass.hlsli"
#include "../../Define/SamplerStateDefine.hlsli"
SamplerState samplerStates[_SAMPLER_STATE_MAX] : register(s0);

struct DS_OUTPUT
{
	float4 vPosition  : SV_POSITION;
};

struct HS_CONTROL_POINT_OUTPUT
{
	float3 vPosition : WORLDPOS; 
};

struct HS_CONSTANT_DATA_OUTPUT
{
	float EdgeTessFactor[3]			: SV_TessFactor;
	float InsideTessFactor			: SV_InsideTessFactor;
};

#define NUM_CONTROL_POINTS 3

[domain("tri")]
GRASS_DS_OUT main(HS_CONSTANT_OUT input, float3 UV : SV_DomainLocation,
const OutputPatch<GRASS_DS_IN, 3> patch)
{
	GRASS_DS_OUT dout;
	
    // 頂点座標
    float3 position = patch[0].position.xyz * UV.x + patch[1].position.xyz * UV.y + patch[2].position.xyz * UV.z;
    // ワールド座標
    float3 worldPosition = patch[0].worldPosition * UV.x + patch[1].worldPosition * UV.y + patch[2].worldPosition * UV.z;
    // 頂点法線
    float3 worldNormal = normalize(patch[0].worldNormal * UV.x + patch[1].worldNormal * UV.y + patch[2].worldNormal * UV.z);
    // タンジェント
    float4 worldTangent = normalize(patch[0].worldTangent * UV.x + patch[1].worldTangent * UV.y + patch[2].worldTangent * UV.z);
    // 頂点UV座標
    float2 texcoord = patch[0].texcoord * UV.x + patch[1].texcoord * UV.y + patch[2].texcoord * UV.z;
    
    float4 parameter = patch[0].parameter * UV.x + patch[1].parameter * UV.y + patch[2].parameter * UV.z;
    // 情報設定
    dout.position = float4(position, 1.0);
    dout.worldPosition = worldPosition;
    dout.worldNormal = worldNormal;
    dout.worldTangent = worldTangent;
    dout.texcoord = texcoord;
    dout.parameter = parameter;
    
    return dout;
}
