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
    float4 grassLodTessFactors;         // LOD用分割数
    
    float grassLODDistance;             // LOD距離
    float perlinNoiseDistributionFactor; // パーリンノイズの分布
    float grassHeightFactor;            // 草の高さ
    float grassHeightVariance;          // 草の高さのズレ
    
    float grassWidthFactor;             // 草の幅
    float grassWidthVariance;           // 草の幅のズレ
    float grassCurvature;               // 曲率
    float grassCurvatureVariance;       // 曲率のズレ
    
    float grassWindVariance;            // 風の強さのズレ
    float grassTaperFactor;             // 草の先細り具合
    float grassTaperVariance;           // 草の先細り具合のズレ
    float grassWitheredFactor;          // 枯れ具合
    
    float obstacleInfluence;            // 障害物の影響度
    float3 padding;

}