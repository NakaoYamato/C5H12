#include "TerrainDeform.hlsli"

VsOut main(in uint vertexid : SV_VERTEXID)
{
    VsOut vout;
    // ブラシのUV座標から描画する場所のUV座標を計算
    const float2 texcoords[4] =
    {
        { brushUVPosition.x - brushRadius, brushUVPosition.y - brushRadius },
        { brushUVPosition.x + brushRadius, brushUVPosition.y - brushRadius },
        { brushUVPosition.x - brushRadius, brushUVPosition.y + brushRadius },
        { brushUVPosition.x + brushRadius, brushUVPosition.y + brushRadius }
    };
    vout.texcoord = texcoords[vertexid];
    vout.position.x = vout.texcoord.x * 2.0f - 1.0f;
    vout.position.y = (1.0f - vout.texcoord.y) * 2.0f - 1.0f;
    vout.position.z = 0.0f;
    vout.position.w = 1.0f;
    vout.color = float4(1.0, 1.0, 1.0, 1.0);
    return vout;
}