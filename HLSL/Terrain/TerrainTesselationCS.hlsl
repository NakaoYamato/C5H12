#include "Terrain.hlsli"
#include "../Define/SamplerStateDefine.hlsli"

// コンピュートシェーダーで生成され、新しいVS/PSが使用する頂点構造体
struct CS_OUT_VERTEX
{
    float4 position : SV_POSITION;
    float3 worldPosition : WORLD_POSITION;
    float3 worldNormal : NORMAL;
    float4 worldTangent : TANGENT;
    float4 parameter : PARAMETER;
    float2 texcoord : TEXCOORD;
};

// 入力データ
StructuredBuffer<VS_IN> InputVertices : register(t0);
StructuredBuffer<uint> InputIndices : register(t1);

// パラメータマップ
Texture2D<float4> parameterTexture : register(t6);
SamplerState pointSampler : register(s0); // SampleLevel用に適切なサンプラーをバインド

// 出力データ (UAV)
RWStructuredBuffer<CS_OUT_VERTEX> OutputVertices : register(u0);
RWStructuredBuffer<uint> OutputIndices : register(u1);
RWStructuredBuffer<uint> DrawArgs : register(u2); // Indirect Draw用引数

// 頂点とインデックスのオフセットを計算するためのヘルパー関数
void GetPatchOffsets(uint groupID, uint tessFactor, out uint vertexOffset, out uint indexOffset)
{
    uint numVertsPerPatch = (tessFactor + 1) * (tessFactor + 2) / 2;
    uint numIndicesPerPatch = tessFactor * tessFactor * 3 * 2; // 分割後の三角形は tessFactor^2 個、四角形ベースで考える
    if (tessFactor > 0)
    {
        numIndicesPerPatch = tessFactor * tessFactor * 3;
    }
    else
    {
        numIndicesPerPatch = 3;
    }


    vertexOffset = groupID * numVertsPerPatch;
    indexOffset = groupID * numIndicesPerPatch;
}

[numthreads(1, 1, 1)]
void main(uint3 Gid : SV_GroupID)
{
    // このスレッドグループが担当する入力三角形(パッチ)のID
    uint patchID = Gid.x;

    // 分割数を定数バッファから取得（簡単のためedgeFactorを使用） [cite: 22]
    uint tessFactor = (uint) edgeFactor;
    if (tessFactor < 1)
        tessFactor = 1;

    // 1. 入力パッチの3頂点を取得
    uint i0 = InputIndices[patchID * 3 + 0];
    uint i1 = InputIndices[patchID * 3 + 1];
    uint i2 = InputIndices[patchID * 3 + 2];
    
    VS_IN v0 = InputVertices[i0];
    VS_IN v1 = InputVertices[i1];
    VS_IN v2 = InputVertices[i2];
    
    // 2. このパッチが出力する頂点とインデックスの書き込み開始位置を計算
    uint vertexOffset;
    uint indexOffset;
    GetPatchOffsets(patchID, tessFactor, vertexOffset, indexOffset);

    uint vertexCounter = 0;

    // 3. ドメインシェーダーのロジックを再現し、新しい頂点を生成
    for (uint y = 0; y <= tessFactor; ++y)
    {
        for (uint x = 0; x <= tessFactor - y; ++x)
        {
            float u = (float) x / tessFactor;
            float v = (float) y / tessFactor;
            float w = 1.0f - u - v;
            
            // ドメインシェーダーの重心座標補間を再現 [cite: 40, 41, 42, 43]
            float3 uvw = float3(w, u, v);
            float2 texcoord = v0.texcoord * uvw.x + v1.texcoord * uvw.y + v2.texcoord * uvw.z;
            float3 position = v0.position.xyz * uvw.x + v1.position.xyz * uvw.y + v2.position.xyz * uvw.z;
            float3 normal = normalize(v0.normal.xyz * uvw.x + v1.normal.xyz * uvw.y + v2.normal.xyz * uvw.z);

            // ドメインシェーダーのハイトマップ適用ロジック [cite: 44, 45, 46, 47]
            float3 worldNormal = normalize(mul(normal, (float3x3) world));
            float4 parameter = parameterTexture.SampleLevel(pointSampler, texcoord, 0);
            float height = parameter.r * heightSclaer;
            
            position = mul(float4(position, 1.0f), world).xyz;
            position += worldNormal * height;

            // 出力頂点構造体を構築 [cite: 48, 49]
            CS_OUT_VERTEX dout;
            dout.position = mul(float4(position, 1.0), viewProjection); // viewProjectionはSceneConstantBufferから
            dout.worldPosition = position;
            dout.worldNormal = worldNormal;
            dout.worldTangent = float4(normalize(cross(worldNormal, float3(0.0f, 1.0f, 0.01f))), 1.0f);
            dout.texcoord = texcoord;
            dout.parameter = parameter;

            OutputVertices[vertexOffset + vertexCounter] = dout;
            vertexCounter++;
        }
    }
    
    // 4. 新しいインデックスを生成
    uint indexCounter = 0;
    for (uint y = 0; y < tessFactor; ++y)
    {
        for (uint x = 0; x < tessFactor - y; ++x)
        {
            // 頂点のインデックスを計算
            uint current_row_start = (y * (2 * tessFactor + 3 - y)) / 2;
            uint next_row_start = ((y + 1) * (2 * tessFactor + 3 - (y + 1))) / 2;
            
            uint i_bottom_left = vertexOffset + current_row_start + x;
            uint i_bottom_right = i_bottom_left + 1;
            uint i_top_left = vertexOffset + next_row_start + x;

            OutputIndices[indexOffset + indexCounter++] = i_bottom_left;
            OutputIndices[indexOffset + indexCounter++] = i_bottom_right;
            OutputIndices[indexOffset + indexCounter++] = i_top_left;

            // 上向きの三角形(四角形の場合)
            if (x < tessFactor - y - 1)
            {
                uint i_top_right = i_top_left + 1;
                OutputIndices[indexOffset + indexCounter++] = i_bottom_right;
                OutputIndices[indexOffset + indexCounter++] = i_top_right;
                OutputIndices[indexOffset + indexCounter++] = i_top_left;
            }
        }
    }

    // 5. Indirect Drawのための引数をアトミックに加算
    //    (IndexCountPerInstance, InstanceCount, StartIndexLocation, BaseVertexLocation, StartInstanceLocation)
    //    ここではIndexCountのみ更新
    if (patchID == 0)
    {
        // 最初のスレッドが引数バッファをリセットする
        DrawArgs[0] = 0; // IndexCount
        DrawArgs[1] = 1; // InstanceCount
        DrawArgs[2] = 0; // StartIndexLocation
        DrawArgs[3] = 0; // BaseVertexLocation
        DrawArgs[4] = 0; // StartInstanceLocation
    }
    
    // メモリバリアを挟んでリセットを待つ
    GroupMemoryBarrierWithGroupSync();

    InterlockedAdd(DrawArgs[0], indexCounter);
}