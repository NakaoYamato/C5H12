#include "Terrain.hlsli"
#include "../../HLSL/Function/Tessellation.hlsli"

// パッチ毎に適用される関数
HS_CONSTANT_OUT HSConstant(
InputPatch<HS_IN, 3> ip,
uint pid : SV_PrimitiveID)
{
    HS_CONSTANT_OUT hout = (HS_CONSTANT_OUT) 0;
#if 1
    // カメラからの距離に応じて分割数を調整
    float4 v0 = mul(float4(ip[0].position.xyz, 1), world);
    float4 v1 = mul(float4(ip[1].position.xyz, 1), world);
    float4 v2 = mul(float4(ip[2].position.xyz, 1), world);
    float4 f = DistanceBasedTess(v0, v1, v2, cameraPosition.xyz, 0.0, lodDistanceMax, max(edgeFactor, innerFactor));
    hout.factor[0] = f.x;
    hout.factor[1] = f.y;
    hout.factor[2] = f.z;
    // 内部部分の分割数を指定
    hout.innerFactor = f.w;
#else
    // エッジの分割数を指定
    hout.factor[0] = edgeFactor;
    hout.factor[1] = edgeFactor;
    hout.factor[2] = edgeFactor;
    // 内部部分の分割数を指定
    hout.innerFactor = innerFactor;
#endif
    
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
    return hout;
}