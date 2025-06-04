#include "../ComputeParticle.hlsli"
#include "../../Function/ToMatrix.hlsli"

// パーティクル管理バッファ
StructuredBuffer<ParticleData> particleDataBuffer : register(t0);
StructuredBuffer<ParticleHeader> particleHeaderBuffer : register(t1);

[maxvertexcount(4)]
void main(point GS_IN gin[1], inout TriangleStream<PS_IN> output)
{
    uint vertexId = gin[0].vertexId;
    
    // ヘッダーのparticle_indexがそのままパーティクルデータバッファのインデックになる
    bool isAlive = particleHeaderBuffer[vertexId].alive != 0;
    vertexId = particleHeaderBuffer[vertexId].particleIndex;
    
    //  生存していない場合はスケールを0にしておく
    float3 scale = !isAlive ? float3(0, 0, 0) : particleDataBuffer[vertexId].scale.xyz;
#if DRAW_BILLBOARD
    //  ビルボード行列生成(ビュー行列の逆行列で良い。ただし移動値はいらない)
    float4x4 billboardMatrix = inverse_view_transform;
    billboardMatrix._41_42_43 = float3(0, 0, 0);
    billboardMatrix._44 = 1.0f;
#endif

	//  ワールド行列生成
    float4x4 scaleMatrix = ToMatrixScaling(scale);
#if DRAW_BILLBOARD
    float4x4 rotationMatrix = mul(billboardMatrix, matrix_rotation_roll_pitch_yaw(particleDataBuffer[vertexId].rotation.xyz));
#else
    float4x4 rotationMatrix = ToMatrixRotationRollPitchYaw(particleDataBuffer[vertexId].rotation.xyz);
#endif
    float4x4 translationMatrix = ToMatrixTranslation(particleDataBuffer[vertexId].position.xyz);
    float4x4 worldMatrix = mul(mul(scaleMatrix, rotationMatrix), translationMatrix);
    float4x4 worldVPMatrix = mul(worldMatrix, viewProjection);

    //  各種情報取得
    float4 texcoord = particleDataBuffer[vertexId].texcoord;
    float4 color = particleDataBuffer[vertexId].color;

    //  頂点生成
    static const float4 vertexPositions[4] =
    {
        float4(-0.5f, -0.5f, 0, 1),
        float4(+0.5f, -0.5f, 0, 1),
        float4(-0.5f, +0.5f, 0, 1),
        float4(+0.5f, +0.5f, 0, 1),
    };
    static const float2 vertexTexcoord[4] =
    {
        float2(0, 0),
        float2(1, 0),
        float2(0, 1),
        float2(1, 1),
    };
    for (uint i = 0; i < 4; i++)
    {
        PS_IN element;
        element.position = mul(vertexPositions[i], worldVPMatrix);
        element.texcoord = texcoord.xy + texcoord.zw * vertexTexcoord[i];
        element.color = color;
        output.Append(element);
    }
    output.RestartStrip();
}