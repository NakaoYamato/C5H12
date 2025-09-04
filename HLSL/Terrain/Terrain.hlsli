#include "../Scene/SceneConstantBuffer.hlsli"

struct VS_IN
{
    float4 position : POSITION;
    float4 normal : NORMAL;
    float2 texcoord : TEXCOORD;
};

// 受け渡し構造体
struct HS_IN
{
    float4 position : SV_POSITION;
    float3 normal : NORMAL;
    float2 texcoord : TECCOORD;
};

// パッチからの出力
struct HS_CONSTANT_OUT
{
    float factor[3] : SV_TessFactor;
    float innerFactor : SV_InsideTessFactor;
};

// コントロールポイントからの出力構造体
#define HS_OUT HS_IN
#define DS_IN HS_IN

struct DS_OUT
{
    float4 position : SV_POSITION;
    float3 worldPosition : WORLD_POSITION;
    float3 worldNormal : NORMAL;
    float4 worldTangent : TANGENT;
    float4 parameter : PARAMETER;// (R：高さ、G：草、B：コスト)
    float2 texcoord : TEXCOORD;
};
#define GS_IN DS_OUT
#define GS_OUT DS_OUT
#define PS_IN DS_OUT
#define StreamOutData DS_OUT

cbuffer TESSELATION_CONSTANT_BUFFER : register(b1)
{
    row_major float4x4 world;
    
    float edgeFactor; // エッジ分割数
    float innerFactor; // 内部分割数
    float heightSclaer; // 高さ係数
    float lodDistanceMax; // LOD(Level Of Detail)距離
    
    float emissive;
    float roughness;
    float metalness;
    float padding;
}