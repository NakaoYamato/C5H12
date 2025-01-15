#include "Sprite.hlsli"
VsOut main(in uint vertexid : SV_VERTEXID)
{
    VsOut vout;
    const float2 position[4] =
    {
        { -1, +1 },
        { +1, +1 },
        { -1, -1 },
        { +1, -1 }
    };
    const float2 texcoords[4] =
    {
        { 0, 0 },
        { 1, 0 },
        { 0, 1 },
        { 1, 1 }
    };
    vout.position = float4(position[vertexid], 0, 1);
    vout.texcoord = texcoords[vertexid];
    vout.color = float4(1.0, 1.0, 1.0, 1.0);
    return vout;
}