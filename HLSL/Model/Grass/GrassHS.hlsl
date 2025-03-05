#include "Grass.hlsli"

// •ªŠ„ŒW”Zoˆ—
//  https://github.com/chsxf/unity-built-in-shaders/blob/07bb37ed606e733343b7dadb2ce3b03421daa4b5/Shaders/CGIncludes/Tessellation.cginc#L81
float CalcDistanceTessFactor(float4 wpos, float minDist, float maxDist, float tess)
{
    float dist = distance(wpos.xyz, camera_position.xyz);
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

HS_CONSTANT_DATA_OUTPUT CalcHSPatchConstants(InputPatch<VS_CONTROL_POINT_OUTPUT, CONTROL_POINT_COUNT> input_patch, uint patch_id : SV_PrimitiveID)
{
    HS_CONSTANT_DATA_OUTPUT hout;

#if 0
    const float subdivision = tesselation_max_subdivision;
#else
    // ƒJƒƒ‰‚©‚ç‚Ì‹——£‚É‰‚¶‚Ä•ªŠ„”‚ğ’²®
    float4 v0 = float4(input_patch[0].world_position.xyz, 1);
    float4 v1 = float4(input_patch[1].world_position.xyz, 1);
    float4 v2 = float4(input_patch[2].world_position.xyz, 1);
    const float subdivision = CalcDistanceTessFactor((v0 + v1 + v2) / 3.0f, 0.0f, lod_distance_max, tesselation_max_subdivision);
#endif
    hout.tess_factor[0] = subdivision;
    hout.tess_factor[1] = subdivision;
    hout.tess_factor[2] = subdivision;
    hout.inside_tess_factor = subdivision;
    
    return hout;
}

[domain("tri")]
[partitioning("integer")] // integer, fractional_odd, fractional_even, pow2
[outputtopology("triangle_cw")]
[outputcontrolpoints(3)]
[patchconstantfunc("CalcHSPatchConstants")]
HS_CONTROL_POINT_OUTPUT main( 
	InputPatch<VS_CONTROL_POINT_OUTPUT, CONTROL_POINT_COUNT> ip,
	uint i : SV_OutputControlPointID,
	uint PatchID : SV_PrimitiveID )
{
	HS_CONTROL_POINT_OUTPUT output;
    output = ip[i];
    return output;
}
