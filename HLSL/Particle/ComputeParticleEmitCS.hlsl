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
    particle_header_buffer[header_index].particleIndex = particleIndex; //  パーティクルデータバッファの座標
    particle_header_buffer[header_index].depth          = 1; //  深度
    particle_header_buffer[header_index].dummy          = 0; //  空き
    
    particleDataBuffer[particleIndex].parameter.x      = emitParticleButter[emit_index].parameter.x;
    particleDataBuffer[particleIndex].parameter.y      = emitParticleButter[emit_index].parameter.y;
    particleDataBuffer[particleIndex].parameter.z      = 0.0f;
    particleDataBuffer[particleIndex].parameter.w      = 0.0f;
    
    particleDataBuffer[particleIndex].position         = emitParticleButter[emit_index].position;
    particleDataBuffer[particleIndex].rotation         = emitParticleButter[emit_index].rotation;
    particleDataBuffer[particleIndex].scale            = emitParticleButter[emit_index].scale;
    particleDataBuffer[particleIndex].velocity         = emitParticleButter[emit_index].velocity;
    particleDataBuffer[particleIndex].acceleration     = emitParticleButter[emit_index].acceleration;
    particleDataBuffer[particleIndex].color            = emitParticleButter[emit_index].color;
}
