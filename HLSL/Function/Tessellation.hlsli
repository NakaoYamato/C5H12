
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

inline float4 CalcTriEdgeTessFactors(float3 triVertexFactors)
{
    float4 tess;
    tess.x = 0.5f * (triVertexFactors.y + triVertexFactors.z);
    tess.y = 0.5f * (triVertexFactors.x + triVertexFactors.z);
    tess.z = 0.5f * (triVertexFactors.x + triVertexFactors.y);
    tess.w = (triVertexFactors.x + triVertexFactors.y + triVertexFactors.z) / 3.0f;
    return tess;
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
#if 1
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
    return CalcTriEdgeTessFactors(f);
#endif
}
