#include "ComputeParticle.hlsli"

 //  パーティクル番号管理バッファ(末尾への追加専用)
AppendStructuredBuffer<int> particleUnusedBuffer : register(u1);
//  パーティクルヘッダー管理バッファ
RWStructuredBuffer<ParticleHeader> particleHeaderBuffer : register(u3);

// パーティクルデータ初期化処理
[numthreads(NumParticleThread, 1, 1)]
void main(uint3 dTid : SV_DispatchThreadID)
{
    int index = dTid.x;

    //  パーティクル情報初期化
    particleHeaderBuffer[index].alive = 0;
    particleHeaderBuffer[index].particleIndex = 0;
 
    //  未使用リスト(AppendStructuredBuffer)の末尾に追加
    particleUnusedBuffer.Append(index);
}
