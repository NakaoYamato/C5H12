#include "Terrain.hlsli"
#include "../Define/SamplerStateDefine.hlsli"
SamplerState samplerStates[_SAMPLER_STATE_MAX] : register(s0);

// パラメータマップ
Texture2D<float4> parameterTexture : register(t2);

[domain("quad")]
DS_OUT main(HS_CONSTANT_OUT input,
float2 UV : SV_DomainLocation,
const OutputPatch<DS_IN, 4> patch)
{
    float u = UV.x;
    float v = UV.y;
    float uBar = 1.0f - u;
    float vBar = 1.0f - v;
    
    float uv = u * v;
    float uBarv = uBar * v;
    float uvBar = u * vBar;
    float uBarvBar = uBar * vBar;
    
    DS_OUT dout = (DS_OUT) 0;
    float3 position =
    patch[0].position * uBarvBar +
    patch[1].position * uBarv +
    patch[2].position * uv +
    patch[3].position * uvBar;
    float3 worldPosition = mul(float4(position, 1.0f), world).xyz;
    // positionをUVに変換
    float2 texcoord = float2(position.x / terrainLength, 1.0f - position.z / terrainLength);
    
    // 現在地点の高さを取得
    float4 parameter = parameterTexture.SampleLevel(samplerStates[_LINEAR_CLAMP_SAMPLER_INDEX], texcoord, 0);
    float height = parameter.r + parameter.b * 0.25f;

    // 微分法を用いて、ハイトマップの形状に沿った正しい法線を計算する
    float w, h;
    parameterTexture.GetDimensions(w, h);
    float2 texelSize = float2(1.0f / w, 1.0f / h);

    // X軸方向(U)とZ軸方向(V)の隣接点の高さを取得
    float4 paramR = parameterTexture.SampleLevel(samplerStates[_LINEAR_CLAMP_SAMPLER_INDEX], texcoord + float2(texelSize.x, 0), 0);
    float4 paramB = parameterTexture.SampleLevel(samplerStates[_LINEAR_CLAMP_SAMPLER_INDEX], texcoord + float2(0, texelSize.y), 0); // Vは下方向(+Y)

    float heightR = paramR.r + paramR.b; // 右隣の高さ
    float heightB = paramB.r + paramB.b; // 下隣の高さ

    // ワールド空間での1テクセルあたりの距離（概算）
    float worldStepX = terrainLength / w;
    float worldStepZ = terrainLength / h;

    // 接線(Tangent)と従法線(Binormal)を計算
    // 高さの差分を使って傾きを求める
    float3 tangent = normalize(float3(worldStepX, heightR - height, 0));
    float3 binormal = normalize(float3(0, heightB - height, -worldStepZ));

    // 新しい法線を計算 (接線と従法線の外積)
    float3 computedNormal = normalize(cross(binormal, tangent));
    
    // ワールド変換
    float3 worldNormal = normalize(mul(computedNormal, (float3x3) world));
    float3 worldTangent = normalize(mul(tangent, (float3x3) world));

    // 座標の適用
    worldPosition.y += height;

    // 情報設定
    dout.position = mul(float4(worldPosition, 1.0), viewProjection);
    dout.worldPosition = worldPosition;
    dout.worldNormal = worldNormal;
    dout.worldTangent = float4(worldTangent, 1.0f);
    dout.texcoord = texcoord;
    dout.parameter = parameter;
    
    return dout;
}
