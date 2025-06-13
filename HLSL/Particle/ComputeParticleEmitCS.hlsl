#include "ComputeParticle.hlsli"

//  パーティクル管理バッファ
RWStructuredBuffer<ParticleData> particleDataBuffer : register(u0);
//  パーティクル番号管理バッファ(末尾から取出専用)
ConsumeStructuredBuffer<uint> parcilePoolBuffer : register(u1);
//  インダイレクト用バッファ
RWByteAddressBuffer indirectDataBuffer : register(u2);
//  パーティクルヘッダー管理バッファ
RWStructuredBuffer<ParticleHeader> particle_header_buffer : register(u3);

//  パーティクル生成情報バッファ
StructuredBuffer<EmitParticleData> emitParticleButter : register(t0);

[numthreads(1, 1, 1)]
void main(uint3 dTid : SV_DispatchThreadID)
{
    //  未使用リストの末尾から未使用パーティクルのインデックスを取得
    uint particleIndex = parcilePoolBuffer.Consume();
    uint emitIndex = dTid.x;
    
    //  ヘッダーの末端から取得
    uint headerIndex = 0;
    indirectDataBuffer.InterlockedAdd(IndirectArgumentsNumEmitParticleIndex, 1, headerIndex);
    
    //  パーティクル生成処理
    particle_header_buffer[headerIndex].alive          = 1; //  生存フラグ
    particle_header_buffer[headerIndex].particleIndex  = particleIndex; //  パーティクルデータバッファの座標
    particle_header_buffer[headerIndex].depth          = 1; //  深度
    particle_header_buffer[headerIndex].dummy          = 0; //  空き
    
    particleDataBuffer[particleIndex].updateType        = emitParticleButter[emitIndex].updateType;
    particleDataBuffer[particleIndex].renderType        = emitParticleButter[emitIndex].renderType;
    particleDataBuffer[particleIndex].lifeTime          = emitParticleButter[emitIndex].timer;
    particleDataBuffer[particleIndex].elapsedTime       = 0.0f;
    particleDataBuffer[particleIndex].texAnimTime       = emitParticleButter[emitIndex].texAnimTime;
    
    particleDataBuffer[particleIndex].position          = emitParticleButter[emitIndex].position;
    particleDataBuffer[particleIndex].startRotation     = emitParticleButter[emitIndex].startRotation;
    particleDataBuffer[particleIndex].endRotation       = emitParticleButter[emitIndex].endRotation;
    particleDataBuffer[particleIndex].startScale        = emitParticleButter[emitIndex].startScale;
    particleDataBuffer[particleIndex].endScale          = emitParticleButter[emitIndex].endScale;
    particleDataBuffer[particleIndex].velocity          = emitParticleButter[emitIndex].velocity;
    particleDataBuffer[particleIndex].acceleration      = emitParticleButter[emitIndex].acceleration;
    particleDataBuffer[particleIndex].startColor        = emitParticleButter[emitIndex].startColor;
    particleDataBuffer[particleIndex].endColor          = emitParticleButter[emitIndex].endColor;
    
    particleDataBuffer[particleIndex].texPosition       = emitParticleButter[emitIndex].texPosition;
    particleDataBuffer[particleIndex].texSize           = emitParticleButter[emitIndex].texSize;
    particleDataBuffer[particleIndex].texSplit          = emitParticleButter[emitIndex].texSplit;
    particleDataBuffer[particleIndex].texcoordIndex     = emitParticleButter[emitIndex].texcoordIndex;
    particleDataBuffer[particleIndex].texAnimTime       = emitParticleButter[emitIndex].texAnimTime;
}
