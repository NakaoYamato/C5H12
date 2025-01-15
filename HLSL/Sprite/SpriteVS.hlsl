#include "Sprite.hlsli"
VsOut main(float4 position : POSITION, float4 color : COLOR, float2 texcoord : TEXCOORD)
{
    VsOut vout;
    vout.position = position;
    vout.color = color;
    vout.texcoord = texcoord;

    return vout;
}