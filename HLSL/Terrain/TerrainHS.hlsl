#include "Terrain.hlsli"

// 分割係数算出処理
//  https://github.com/chsxf/unity-built-in-shaders/blob/07bb37ed606e733343b7dadb2ce3b03421daa4b5/Shaders/CGIncludes/Tessellation.cginc#L81
float CalcDistanceTessFactor(float4 position, float minDist, float maxDist, float tess)
{
    float3 wpos = mul(position, world).xyz;
    float dist = distance(wpos, cameraPosition.xyz);
    float f = clamp(1.0 - (dist - minDist) / (maxDist - minDist), 0.001f, 1.0f) * tess;
    return f;
}

float4 CalcTriEdgeTessFactors(float3 triVertexFactors)
{
    float4 tess;
    tess.x = 0.5f * (triVertexFactors.y + triVertexFactors.z);
    tess.y = 0.5f * (triVertexFactors.x + triVertexFactors.z);
    tess.z = 0.5f * (triVertexFactors.x + triVertexFactors.y);
    tess.w = (triVertexFactors.x + triVertexFactors.y + triVertexFactors.z) / 3.0f;
    return tess;
}

float4 DistanceBasedTess(float4 v0, float4 v1, float4 v2, float minDist, float maxDist, float tess)
{
    float3 f;
    f.x = CalcDistanceTessFactor(v0, minDist, maxDist, tess);
    f.y = CalcDistanceTessFactor(v1, minDist, maxDist, tess);
    f.z = CalcDistanceTessFactor(v2, minDist, maxDist, tess);
    return CalcTriEdgeTessFactors(f);
}

// パッチ毎に適用される関数
HS_CONSTANT_OUT HSConstant(
InputPatch<HS_IN, 3> ip,
uint pid : SV_PrimitiveID)
{
    HS_CONSTANT_OUT hout = (HS_CONSTANT_OUT) 0;
    // カメラからの距離に応じて分割数を調整
    float4 v0 = float4(ip[0].position.xyz, 1);
    float4 v1 = float4(ip[1].position.xyz, 1);
    float4 v2 = float4(ip[2].position.xyz, 1);
    //float4 f = DistanceBasedTess(v0, v1, v2, 0.0, lodDistanceMax, max(edgeFactor, innerFactor));
    //hout.factor[0] = f.x;
    //hout.factor[1] = f.y;
    //hout.factor[2] = f.z;
    //// 内部部分の分割数を指定
    //hout.innerFactor = f.w;    
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
    return hout;
}