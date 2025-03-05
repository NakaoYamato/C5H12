#include "Grass.hlsli"

[domain("tri")]
DS_OUT main(
	HS_CONSTANT_DATA_OUTPUT input,
	float3 domain : SV_DomainLocation,
	const OutputPatch<HS_CONTROL_POINT_OUTPUT, CONTROL_POINT_COUNT> patch)
{
    DS_OUT output;
    output.world_position = patch[0].world_position * domain.x + patch[1].world_position * domain.y + patch[2].world_position * domain.z;
    output.world_position.w = 1;

    return output;
}
