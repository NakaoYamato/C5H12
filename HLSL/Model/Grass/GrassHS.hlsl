#include "Grass.hlsli"

HS_CONSTANT_DATA_OUTPUT CalcHSPatchConstants(InputPatch<VS_CONTROL_POINT_OUTPUT, CONTROL_POINT_COUNT> input_patch, uint patch_id : SV_PrimitiveID)
{
    HS_CONSTANT_DATA_OUTPUT output;

    const float subdivision = tesselation_max_subdivision;
    output.tess_factor[0] = subdivision;
    output.tess_factor[1] = subdivision;
    output.tess_factor[2] = subdivision;
    output.inside_tess_factor = subdivision;
    return output;
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
