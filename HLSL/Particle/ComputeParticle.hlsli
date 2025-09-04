#include "../Scene/SceneConstantBuffer.hlsli"

// パーティクルスレッド数
static const int NumParticleThread = 1024;

// パーティクル更新タイプ
static const uint ParticleUpdateTypeDefault = 0;

// パーティクル描画タイプ
static const uint ParticleRenderTypeDefault = 0;
static const uint ParticleRenderTypeBillboard = 1;

// パーティクルのテクスチャ開始番号
#define ParticleTextureStartIndex t30

// 生成パーティクル構造体
struct EmitParticleData
{
	// 更新タイプ
    uint updateType;
    // 描画タイプ
    uint renderType;
    // 生存時間
    float timer;
    float padding_0;
    
    // 生成座標
    float4 position;
    // 回転情報
    float4 startRotation;
    float4 endRotation;
    // 拡縮情報
    float4 startScale;
    float4 endScale;
    // 初速
    float4 velocity;
    // 加速度
    float4 acceleration;
    // 初期色
    float4 startColor;
    // 終了色
    float4 endColor;
    
	// テクスチャ座標
    float2 texPosition;
	// テクスチャの大きさ
    float2 texSize;
	// テクスチャの分割数
    uint2 texSplit;
    // テクスチャ切り取り番号
    uint texcoordIndex;
	// テクスチャアニメーションの速度
    float texAnimTime;
};

// パーティクル構造体
struct ParticleData
{
	// 更新タイプ
    uint updateType;
    // 描画タイプ
    uint renderType;
    // 生存時間
    float lifeTime;
    // 経過時間
    float elapsedTime;
    
    // 生成座標
    float4 position;
    // 回転情報
    float4 startRotation;
    float4 endRotation;
    float4 rotation;
    // 拡縮情報
    float4 startScale;
    float4 endScale;
    float4 scale;
    // 初速
    float4 velocity;
    // 加速度
    float4 acceleration;
    // 初期色
    float4 startColor;
    // 終了色
    float4 endColor;
    // 現在の色
    float4 color;
    
    // テクスチャ座標(uv)
    float4 texcoord;
	// テクスチャ座標
    float2 texPosition;
	// テクスチャの大きさ
    float2 texSize;
	// テクスチャの分割数
    uint2 texSplit;
    // テクスチャ切り取り番号
    uint texcoordIndex;
	// テクスチャアニメーションの速度
    float texAnimTime;
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
    float2  canvasSize;
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
    float4 color    : COLOR;
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