#include "../Scene/SceneConstantBuffer.hlsli"

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
    float3 normal : NORMAL;
    float2 texcoord : TEXCOORD;
    float4 blendRate : BLEND_RATE; // ブレンド率（R:岩, G:土, B:草, A:基本色）
};
#define GS_IN DS_OUT
#define GS_OUT DS_OUT
#define PS_IN DS_OUT

cbuffer TESSELATION_CONSTANT_BUFFER : register(b1)
{
    row_major float4x4 world;
    
    float4 baseColor; // ベースカラー
    
    float edgeFactor; // エッジ分割数
    float innerFactor; // 内部分割数
    float heightSclaer; // 高さ係数
    float tillingScale; // タイリング係数
    
    float emissive;
    float roughness;
    float metalness;
    float padding;
}