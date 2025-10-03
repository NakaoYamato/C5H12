#include "PrimitiveRenderer.hlsli"
#include "../CBuffer/B0/SceneCB.hlsli"

VS_OUT main(VS_IN vin, uint vertexId : SV_VertexID)
{
    VS_OUT vout;
    vout.position = mul(vin.position, viewProjection);
    vout.texcoord.x = ((float) vertexId / 2.0f / (float) (vertexCount / 2));
    vout.texcoord.y = (float) (vertexId % 2);
    vout.color = vin.color;

    return vout;
}