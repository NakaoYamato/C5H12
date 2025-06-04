#include "ComputeParticle.hlsli"

//  パーティクル管理バッファ
RWStructuredBuffer<ParticleData> particleDataBuffer : register(u0);
//  パーティクル番号管理バッファ(末尾への追加専用)
AppendStructuredBuffer<uint> particleUnusedBuffer : register(u1);
// インダイレクト用バッファ
RWByteAddressBuffer indirectDataBuffer : register(u2);
//  パーティクルヘッダー管理バッファ
RWStructuredBuffer<ParticleHeader> particleHeaderBuffer : register(u3);

[numthreads(NumParticleThread, 1, 1)]
void main(uint3 dTid : SV_DispatchThreadID)
{
    uint headerIndex = dTid.x;
    
    ParticleHeader header = particleHeaderBuffer[headerIndex];
    
    uint dataIndex = header.particleIndex;
    
    // 有効フラグが立っているものだけ処理
    if (header.alive == 0)
        return;
    
    ParticleData data = particleDataBuffer[dataIndex];
    
    // 経過時間処理
    data.parameter.y -= elapsedTime;
    if (data.parameter.y < 0.0)
    {
        // 寿命が尽きたら未使用リストに追加
        header.alive = 0;
        particleUnusedBuffer.Append(dataIndex);
        
        // ヘッダー情報更新
        particleHeaderBuffer[headerIndex] = header;
        
        // 死亡数をカウント
        uint original_count;
        indirectDataBuffer.InterlockedAdd(IndirectArgumentsNumDeadParticle, 1, original_count);
        return;
    }
    
    // 速度更新
    data.velocity.xyz += data.acceleration.xyz * elapsedTime;
    
    // 位置更新
    data.position.xyz += data.velocity.xyz * elapsedTime;
    
    //  切り取り座標を算出
    uint type = (uint) (data.parameter.x + 0.5f);
    float w = 1.0 / textureSplitCount.x;
    float h = 1.0 / textureSplitCount.y;
    float2 uv = float2((type % textureSplitCount.x) * w, (type / textureSplitCount.x) * h);
    data.texcoord.xy = uv;
    data.texcoord.zw = float2(w, h);
        
    //  徐々に透明にしていく
    data.color.a = saturate(data.parameter.y);
    
    // 深度ソート値算出
    header.depth = mul(float4(data.position.xyz, 1), viewProjection).w;
    
    // 更新情報を格納
    particleHeaderBuffer[headerIndex] = header;
    particleDataBuffer[dataIndex] = data;
}
