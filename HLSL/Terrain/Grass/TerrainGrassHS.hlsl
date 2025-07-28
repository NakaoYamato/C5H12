#include "TerrainGrass.hlsli"

// パッチ毎に適用される関数
HS_CONSTANT_OUT HSConstant(
InputPatch<GRASS_HS_IN, 3> ip,
uint pid : SV_PrimitiveID)
{
    HS_CONSTANT_OUT hout = (HS_CONSTANT_OUT) 0;
    hout.factor[0] = grassTessellation;
    hout.factor[1] = grassTessellation;
    hout.factor[2] = grassTessellation;
    // 内部部分の分割数を指定
    hout.innerFactor = grassTessellation;
    
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
