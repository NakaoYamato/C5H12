
// 分割係数算出処理
//  https://github.com/chsxf/unity-built-in-shaders/blob/07bb37ed606e733343b7dadb2ce3b03421daa4b5/Shaders/CGIncludes/Tessellation.cginc#L81
inline float CalcDistanceTessFactor(
float4 worldPosition,
float3 cameraPosition,
float minDist,
float maxDist,
float tess)
{
    float dist = distance(worldPosition.xyz, cameraPosition.xyz);
    float f = clamp(1.0 - (dist - minDist) / (maxDist - minDist), 0.001f, 1.0f) * tess;
    return f;
}
// 距離に基づくテッセレーション係数を計算する関数
// v0, v1, v2: ワールド座標系での頂点位置
inline float4 DistanceBasedTess(
float4 v0, float4 v1, float4 v2,
float3 cameraPosition, 
float minDist, 
float maxDist, 
float tess)
{
    float3 f;    
#if 0
    // 中心のみでテッセレーション係数を計算
    float4 center = (v0 + v1 + v2) / 3.0f;
    float factor = CalcDistanceTessFactor(center, cameraPosition, minDist, maxDist, tess);
    f.x = factor;
    f.y = factor;
    f.z = factor;
    return float4(f, factor);
#else
    f.x = CalcDistanceTessFactor(v0, cameraPosition, minDist, maxDist, tess);
    f.y = CalcDistanceTessFactor(v1, cameraPosition, minDist, maxDist, tess);
    f.z = CalcDistanceTessFactor(v2, cameraPosition, minDist, maxDist, tess);
    float3 rtf;
    float ritf, itf;
    ProcessTriTessFactorsAvg(f, (f.x + f.y + f.z) / 3.0f, rtf, ritf, itf);
    return float4(rtf, ritf);
#endif
}
inline void DistanceBasedTessQuad(
float4 v0, float4 v1, float4 v2, float4 v3,
float3 cameraPosition, 
float minDist, 
float maxDist, 
float tess,
out float4 edgeFactors,
out float2 insideFactors)
{
    edgeFactors = float4(1, 1, 1, 1);
    insideFactors = float2(1, 1);
    float4 f;
    f.x = CalcDistanceTessFactor(v0, cameraPosition, minDist, maxDist, tess);
    f.y = CalcDistanceTessFactor(v1, cameraPosition, minDist, maxDist, tess);
    f.z = CalcDistanceTessFactor(v2, cameraPosition, minDist, maxDist, tess);
    f.w = CalcDistanceTessFactor(v3, cameraPosition, minDist, maxDist, tess);
    float2 unroundInsideFactors;
    ProcessQuadTessFactorsAvg(f,
    (f.x + f.y + f.z + f.w) / 4.0f,
    edgeFactors,
    insideFactors, 
    unroundInsideFactors);
}
