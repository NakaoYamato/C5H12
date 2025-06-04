#include "ComputeParticle.hlsli"

RWByteAddressBuffer indirectDataBuffer : register(u2);

[numthreads(1, 1, 1)]
void main(uint3 dTid : SV_DispatchThreadID)
{
    // 1F前の総パーティクル数＋現在のフレームの生成パーティクル数　＝　”仮の”現在の総パーティクル数
    uint previousNumParticle = indirectDataBuffer.Load(IndirectArgumentsNumCurrentParticle);
    uint currentNumParticle = previousNumParticle + totalEmitCount;
    
    // 現在フレームの総パーティクル数はシステムの総パーティクル数で制限
    currentNumParticle = min(systemNumParticles, currentNumParticle);
    
    // 総数を記録
    indirectDataBuffer.Store(IndirectArgumentsNumCurrentParticle, currentNumParticle);
    indirectDataBuffer.Store(IndirectArgumentsNumPreviousParticle, previousNumParticle);
    
    // 死亡カウンターを初期化
    indirectDataBuffer.Store(IndirectArgumentsNumDeadParticle, 0);
    
    // エミッター用のdispatch indirectに起動数を設定
    uint3 emit_dispatch = (uint3) 0;
    emit_dispatch.x = currentNumParticle - previousNumParticle;
    emit_dispatch.y = 1;
    emit_dispatch.z = 1;
    indirectDataBuffer.Store3(IndirectArgumentsEmitParticleDispatchIndirect, emit_dispatch);
    
    // エミッターの生成番号を設定
    indirectDataBuffer.Store(IndirectArgumentsNumEmitParticleIndex, previousNumParticle);
    
    // 更新用のdispatch indirectに起動数を設定
    uint3 updateDispatch = (uint3) 0;
    updateDispatch.x = ((currentNumParticle + (NumParticleThread - 1)) / NumParticleThread);
    updateDispatch.y = 1;
    updateDispatch.z = 1;
    indirectDataBuffer.Store3(IndirectArgumentsUpdateParticleDispatchIndirect, updateDispatch);
}
