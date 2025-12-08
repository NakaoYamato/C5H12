#include "../CBuffer/B0/SceneCB.hlsli"

// VS入力
struct VS_IN
{
    float3 position : POSITION;
};

// HS入力(VS出力と同じ)
#define HS_IN VS_IN

// パッチからの出力
struct HS_CONSTANT_OUT
{
    float factor[4] : SV_TessFactor;
    float innerFactor[2] : SV_InsideTessFactor;
};

// HS出力(VS出力と同じ)
#define HS_OUT HS_IN
// DS入力(VS出力と同じ)
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
    
    float4 lodTessFactors;    // LOD用分割数
    
    int divisionCount;          // 分割数
    float terrainLength;        // 全体の長さ
    float lodDistance;          // LOD距離
    float collisionTessFactor;  // 衝突判定用エッジ分割数(奇数のみ)    
    float emissive;
    
    float metalness;
    float roughness;
    float tessPadding[2];
}