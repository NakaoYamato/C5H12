struct VS_IN
{
    float4 position : POSITION;
    float4 color : COLOR;
};

struct VS_OUT
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD;
    float4 color : COLOR;
};

cbuffer CbPrimitive : register(b1)
{
    uint vertexCount;
    float2 viewportSize;
    float padding;
};