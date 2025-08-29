#include "Terrain.hlsli"
#include "../../HLSL/Function/Tessellation.hlsli"

// パッチ毎に適用される関数
HS_CONSTANT_OUT HSConstant(
InputPatch<HS_IN, 4> ip,
uint pid : SV_PrimitiveID)
{
    HS_CONSTANT_OUT hout = (HS_CONSTANT_OUT) 0;
    // カメラからの距離に応じて分割数を調整
    float4 v0 = mul(float4(ip[0].position.xyz, 1), world);
    float4 v1 = mul(float4(ip[1].position.xyz, 1), world);
    float4 v2 = mul(float4(ip[2].position.xyz, 1), world);
    float4 v3 = mul(float4(ip[3].position.xyz, 1), world);
    float4 edgeFactors;
    float2 insideFactors;
    DistanceBasedTessQuad(
    v0, v1, v2, v3, 
    cameraPosition.xyz,
    0.0,
    lodDistanceMax,
    max(edgeFactor, innerFactor),
    edgeFactors,
    insideFactors);
    // エッジの分割数を指定
    hout.factor[0] = max(edgeFactors.x, lodLowFactor);
    hout.factor[1] = max(edgeFactors.y, lodLowFactor);
    hout.factor[2] = max(edgeFactors.z, lodLowFactor);
    hout.factor[3] = max(edgeFactors.w, lodLowFactor);
    // 内部部分の分割数を指定
    hout.innerFactor[0] = max(insideFactors.x, lodLowFactor);
    hout.innerFactor[1] = max(insideFactors.y, lodLowFactor);
    
    return hout;
}

// コントロールポイントごとに適用される関数
[domain("quad")]
[partitioning("fractional_odd")] // integer fractional_odd fractional_even
[outputtopology("triangle_ccw")]
[outputcontrolpoints(4)]
[patchconstantfunc("HSConstant")]
DS_IN main(InputPatch<HS_IN, 4> input,
uint cpid : SV_OutputControlPointID)
{
    DS_IN hout = (DS_IN) 0;
    hout.position = input[cpid].position;
    hout.texcoord = input[cpid].texcoord;
    hout.normal = input[cpid].normal;
    return hout;
}