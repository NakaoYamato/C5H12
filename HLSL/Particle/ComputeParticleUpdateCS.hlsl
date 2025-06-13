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
    
    // パーティクルデータ取得
    ParticleData data = particleDataBuffer[dataIndex];
    
    // 経過時間処理
    float oldElapsedTime = data.elapsedTime;
    data.elapsedTime    += elapsedTime;
    if (data.elapsedTime > data.lifeTime)
    {
        // 寿命が尽きたら未使用リストに追加
        header.alive = 0;
        particleUnusedBuffer.Append(dataIndex);
        
        // ヘッダー情報更新
        particleHeaderBuffer[headerIndex] = header;
        
        // 死亡数をカウント
        uint originalCount;
        indirectDataBuffer.InterlockedAdd(IndirectArgumentsNumDeadParticle, 1, originalCount);
        return;
    }
    
    // タイマーの割合を算出
    float timerRate = data.elapsedTime / data.lifeTime;
    
    // 速度更新
    data.velocity.xyz += data.acceleration.xyz * elapsedTime;
    
    // 位置更新
    data.position.xyz += data.velocity.xyz * elapsedTime;
    
    // 回転更新
    data.rotation = lerp(data.startRotation, data.endRotation, timerRate);
    
    // 拡縮更新
    data.scale = lerp(data.startScale, data.endScale, timerRate);
    
    // アニメーション処理
    if (data.texAnimTime > 0.0f)
    {
        uint oldValue = (uint) (oldElapsedTime / data.texAnimTime);
        uint currentValue = (uint) (data.elapsedTime / data.texAnimTime);
        
        if (oldValue != currentValue)
        {
            // テクスチャアニメーションの更新
            data.texcoordIndex++;
        }
    }
    
    //  切り取り座標を算出
    float w = (data.texSize.x / data.texSplit.x) / canvasSize.x;
    float h = (data.texSize.y / data.texSplit.y) / canvasSize.y;
    float2 uv = float2(
    data.texPosition.x / canvasSize.x + (uint)(data.texcoordIndex % data.texSplit.x) * w,
    data.texPosition.y / canvasSize.y + (uint)(data.texcoordIndex / data.texSplit.x) * h);
    data.texcoord.xy = uv;
    data.texcoord.zw = float2(w, h);
    
    // 色の補間
    data.color = lerp(data.startColor, data.endColor, timerRate);
    
    // 深度ソート値算出
    header.depth = mul(float4(data.position.xyz, 1), viewProjection).w;
    
    // 更新情報を格納
    particleHeaderBuffer[headerIndex] = header;
    particleDataBuffer[dataIndex] = data;
}
