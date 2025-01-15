#include "CascadedShadow.hlsli"

[maxvertexcount(CASCADED_SHADOW_MAPS_SIZE)]
void main(triangle VS_OUT_CSM input[3], inout TriangleStream<GS_OUTPUT_CSM> output)
{
    GS_OUTPUT_CSM element;
    element.render_target_array_index = input[0].instance_id;

    element.position = input[0].position;
    output.Append(element);
    element.position = input[1].position;
    output.Append(element);
    element.position = input[2].position;
    output.Append(element);

    output.RestartStrip();
}
