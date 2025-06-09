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
    uint emit_index = dTid.x;
    
    //  ヘッダーの末端から取得
    uint header_index = 0;
    indirectDataBuffer.InterlockedAdd(IndirectArgumentsNumEmitParticleIndex, 1, header_index);
    
    //  パーティクル生成処理
    particle_header_buffer[header_index].alive          = 1; //  生存フラグ
    particle_header_buffer[header_index].particleIndex  = particleIndex; //  パーティクルデータバッファの座標
    particle_header_buffer[header_index].depth          = 1; //  深度
    particle_header_buffer[header_index].dummy          = 0; //  空き
    
    particleDataBuffer[particleIndex].updateType        = emitParticleButter[emit_index].updateType;
    particleDataBuffer[particleIndex].renderType        = emitParticleButter[emit_index].renderType;
    particleDataBuffer[particleIndex].lifeTime          = emitParticleButter[emit_index].timer;
    particleDataBuffer[particleIndex].elapsedTime       = 0.0f;
    particleDataBuffer[particleIndex].texAnimTime       = emitParticleButter[emit_index].texAnimTime;
    
    particleDataBuffer[particleIndex].position          = emitParticleButter[emit_index].position;
    particleDataBuffer[particleIndex].startRotation     = emitParticleButter[emit_index].startRotation;
    particleDataBuffer[particleIndex].endRotation       = emitParticleButter[emit_index].endRotation;
    particleDataBuffer[particleIndex].startScale        = emitParticleButter[emit_index].startScale;
    particleDataBuffer[particleIndex].endScale          = emitParticleButter[emit_index].endScale;
    particleDataBuffer[particleIndex].velocity          = emitParticleButter[emit_index].velocity;
    particleDataBuffer[particleIndex].acceleration      = emitParticleButter[emit_index].acceleration;
    particleDataBuffer[particleIndex].startColor        = emitParticleButter[emit_index].startColor;
    particleDataBuffer[particleIndex].endColor          = emitParticleButter[emit_index].endColor;
    
    particleDataBuffer[particleIndex].texPosition       = emitParticleButter[emit_index].texPosition;
    particleDataBuffer[particleIndex].texSize           = emitParticleButter[emit_index].texSize;
    particleDataBuffer[particleIndex].texSplit          = emitParticleButter[emit_index].texSplit;
    particleDataBuffer[particleIndex].texcoordIndex     = emitParticleButter[emit_index].texcoordIndex;
    particleDataBuffer[particleIndex].texAnimTime       = emitParticleButter[emit_index].texAnimTime;
}
