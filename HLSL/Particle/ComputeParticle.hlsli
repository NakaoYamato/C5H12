#include "../Scene/SceneConstantBuffer.hlsli"

// パーティクルスレッド数
static const int NumParticleThread = 1024;

// 生成パーティクル構造体
struct EmitParticleData
{
    float4 parameter; // x : パーティクル処理タイプ, y : 生存時間, zw : 空き
    
    float4 position; // 生成座標
    float4 rotation; // 回転情報
    float4 scale; // 拡縮情報
    
    float4 velocity; // 初速
    float4 acceleration; // 加速度
    
    float4 color; // 色情報
};

// パーティクル構造体
struct ParticleData
{
    float4 parameter; // x : パーティクル処理タイプ, y : 生存時間, z,w : 空き
    
    float4 position; // 生成座標
    float4 rotation; // 回転情報
    float4 scale; // 拡縮情報
    
    float4 velocity; // 初速
    float4 acceleration; // 加速度
    
    float4 texcoord; // UV情報
    float4 color; // 色情報
};

// パーティクルヘッダー構造体
struct ParticleHeader
{
    uint    alive; // 生存フラグ
    uint    particleIndex; // パーティクル番号
    float   depth; // 深度
    uint    dummy;
};

// IndirectDataBuffer へのアクセス用バイトオフセット
static const uint IndirectArgumentsNumCurrentParticle = 0;
static const uint IndirectArgumentsNumPreviousParticle = 4;
static const uint IndirectArgumentsNumDeadParticle = 8;
static const uint IndirectArgumentsEmitParticleDispatchIndirect = 12;

// DrawInstanced用DrawIndirect用構造体
struct DrawIndirect
{
    uint vertexCountPerInstance;
    uint instanceCount;
    uint startVertexLocation;
    uint startInstanceLocation;
};
static const uint IndirectArgumentsUpdateParticleDispatchIndirect = 24;
static const uint IndirectArgumentsNumEmitParticleIndex = 36;
static const uint IndirectArgumentsDrawIndirect = 40;

//======================================================
// 汎用情報
cbuffer COMPUTE_PARTICLE_COMMON_CONSTANT_BUFFER : register(b10)
{
    float   elapsedTime;
    uint2   textureSplitCount;
    uint    systemNumParticles;
    // 生成予定のパーティクル数
    uint    totalEmitCount;
    uint    commonDummy[3];
}

//======================================================
// 頂点シェーダーからジオメトリシェーダーに転送する情報
struct GS_IN
{
    uint vertexId : VERTEX_ID;
};

// ジオメトリシェーダーからピクセルシェーダーに転送する情報
struct PS_IN
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
    float2 texcoord : TEXCOORD;
};

// バイトニックソート
cbuffer COMPUTE_PARTICLE_BITONIC_SORT_CONSTANT_BUFFER : register(b11)
{
    uint increment;
    uint direction;
    uint sortDummy[2];
}
static const uint BitonicSortB2Thread = 256;
static const uint BitonicSortC2Thread = 512;