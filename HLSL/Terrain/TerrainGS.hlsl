#include "Terrain.hlsli"

//  単純にストリームアウトを利用するためのシェーダーなのでそのまま渡す
[maxvertexcount(3)]
void main(
	triangle GS_IN gin[3] : SV_POSITION,
	inout TriangleStream<GS_OUT> gout
)
{
    for (uint i = 0; i < 3; i++)
    {
		GS_OUT elemant;
        elemant.position = gin[i].position;
        elemant.texcoord = gin[i].texcoord;
        elemant.normal = gin[i].normal;
        elemant.color = gin[i].color;
        elemant.worldPosition = gin[i].worldPosition;
        elemant.blendRate = gin[i].blendRate;
        gout.Append(elemant);
    }
    gout.RestartStrip();
}