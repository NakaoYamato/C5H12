#include "../Scene/SceneConstantBuffer.hlsli"

struct VS_IN
{
    float4 position : POSITION;
    float4 boneWeights : WEIGHTS;
    uint4 boneIndices : BONES;
    float2 texcoord : TEXCOORD;
    float4 normal : NORMAL;
    float4 tangent : TANGENT;
};
struct VS_OUT
{
    float4 position : SV_POSITION;
    float4 world_position : POSITION;
    float4 world_normal : NORMAL;
    float4 world_tangent : TANGENT;
    float2 texcoord : TEXCOORD;
    float4 binormal : BINORMAL;
    float4 materialColor : COLOR;
};

// スケルトン用定数バッファはSkinning.hlsli側で1番に設定
