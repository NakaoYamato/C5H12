#include "TerrainGrass.hlsli"
#include "../../Function/Tessellation.hlsli"
#include "../../Function/Noise.hlsli"

// パッチ毎に適用される関数
GRASS_HS_CONSTANT_OUT HSConstant(
InputPatch<GRASS_HS_IN, 3> ip,
uint pid : SV_PrimitiveID)
{
    GRASS_HS_CONSTANT_OUT hout = (GRASS_HS_CONSTANT_OUT) 0;
    float4 v0 = float4(ip[0].worldPosition.xyz, 1);
    float4 v1 = float4(ip[1].worldPosition.xyz, 1);
    float4 v2 = float4(ip[2].worldPosition.xyz, 1);
    float4 f = DistanceBasedTess(v0, v1, v2, cameraPosition.xyz, 0.0, grassLODDistanceMax, grassTessellation);
    hout.factor[0] = f.x;
    hout.factor[1] = f.y;
    hout.factor[2] = f.z;
    // 内部部分の分割数を指定
    hout.innerFactor = /*Random(ip[0].worldPosition.xz) **/ f.w;
    
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
