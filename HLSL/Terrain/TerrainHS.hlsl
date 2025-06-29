#include "Terrain.hlsli"

// パッチ毎に適用される関数
HS_CONSTANT_OUT HSConstant(
InputPatch<HS_IN, 3> ip,
uint pid : SV_PrimitiveID)
{
    HS_CONSTANT_OUT hout = (HS_CONSTANT_OUT) 0;
    // エッジの分割数を指定
    hout.factor[0] = edgeFactor;
    hout.factor[1] = edgeFactor;
    hout.factor[2] = edgeFactor;
    // 内部部分の分割数を指定
    hout.innerFactor = innerFactor;
    
    return hout;
}

// コントロールポイントごとに適用される関数
[domain("tri")]
[partitioning("integer")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(3)]
[patchconstantfunc("HSConstant")]
DS_IN main(InputPatch<HS_IN, 3> input,
uint cpid : SV_OutputControlPointID,
uint pid : SV_PrimitiveID)
{
    DS_IN hout = (DS_IN) 0;
    hout.position = input[cpid].position;
    hout.texcoord = input[cpid].texcoord;
    hout.normal = input[cpid].normal;
    hout.color = input[cpid].color;
    return hout;
}