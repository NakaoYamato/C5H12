#include "TerrainGrass.hlsli"
#include "../../Function/Tessellation.hlsli"
#include "../../Function/Noise.hlsli"

// パッチ毎に適用される関数
GRASS_HS_CONSTANT_OUT HSConstant(
InputPatch<GRASS_HS_IN, 3> ip,
uint pid : SV_PrimitiveID)
{
    GRASS_HS_CONSTANT_OUT hout = (GRASS_HS_CONSTANT_OUT) 0;
    // カメラからの距離に応じて分割数を調整
    float3 center = (ip[0].worldPosition + ip[1].worldPosition + ip[2].worldPosition) / 4.0;
    float3 vec = center - cameraPosition.xyz;
    float len = length(vec);
    int index = (int) clamp(len / lodDistance, 0.0f, 3.0f);
    float factor = grassLodTessFactors[index];
    
    hout.factor[1] = factor;
    hout.factor[2] = factor;
    hout.innerFactor = factor;
    
    return hout;
}

[domain("tri")]
[partitioning("integer")]
[outputtopology("point")]
[outputcontrolpoints(3)]
[patchconstantfunc("HSConstant")]
GRASS_DS_IN main(InputPatch<GRASS_HS_IN, 3> input,
uint cpid : SV_OutputControlPointID,
uint pid : SV_PrimitiveID)
{
    GRASS_DS_IN hout = (GRASS_DS_IN) 0;
    hout.position = input[cpid].position;
    hout.worldPosition = input[cpid].worldPosition;
    hout.worldNormal = input[cpid].worldNormal;
    hout.worldTangent = input[cpid].worldTangent;
    hout.texcoord = input[cpid].texcoord;
    hout.parameter = input[cpid].parameter;
    return hout;
}
