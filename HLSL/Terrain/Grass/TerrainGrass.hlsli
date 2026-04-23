#include "../Terrain.hlsli"

struct GRASS_PS_IN
{
    float4 position : SV_POSITION; // 位置
    float4 worldPosition : WORLD_POSITION; // ワールド空間の位置
    float3 worldNormal : NORMAL; // ワールド空間の法線
    float2 texcoord : TEXCOORD; // テクスチャ座標
    float4 color : COLOR; // 色
};

// パッチからの出力
struct GRASS_HS_CONSTANT_OUT
{
    float factor[3] : SV_TessFactor;
    float innerFactor : SV_InsideTessFactor;
};

#define GRASS_HS_IN StreamOutData
#define GRASS_HS_OUT StreamOutData
#define GRASS_DS_IN StreamOutData
#define GRASS_DS_OUT StreamOutData
#define GRASS_GS_IN StreamOutData

// 草描画用定数バッファ
cbuffer TERRAIN_GRASS_CONSTANT_BUFFER : register(b2)
{
    float grassTessellation;
    float grassLODDistanceMax;
    float grassHeightFactor;
    float grassWidthFactor;
    
    float grassWitheredFactor;
    float grassCurvature;
    float grassHeightVariance;
    float perlinNoiseDistributionFactor;
    
    float obstacleInfluence;
    float3 padding;

}