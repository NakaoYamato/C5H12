#include "Terrain.hlsli"
#include "../Define/SamplerStateDefine.hlsli"
SamplerState samplerStates[_SAMPLER_STATE_MAX] : register(s0);

// パラメータマップ
Texture2D<float4> parameterTexture : register(t6);

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
    // 法線計算
    float3 normal = normalize(cross(patch[3].position - patch[1].position, patch[0].position - patch[1].position));
    // positionをUVに変換
    float2 texcoord = float2((position.x + 1.0f) / 2.0f, 1.0f - (position.z + 1.0f) / 2.0f);
    
    // ワールド法線
    float3 worldNormal = normalize(mul(normal, (float3x3) world));
    // パラメータマップから高さ方向取得
    float4 parameter = parameterTexture.SampleLevel(samplerStates[_POINT_CLAMP_SAMPLER_INDEX], texcoord, 0);
    float height = parameter.r;
    // 頂点座標をハイトマップで取得した値分ずらす
    {
        worldPosition.y += height;
    }
    // 情報設定
    dout.position = mul(float4(worldPosition, 1.0), viewProjection);
    dout.worldPosition = worldPosition;
    dout.worldNormal = worldNormal;
    dout.worldTangent = float4(normalize(cross(worldNormal, float3(0.0f, 1.0f, 0.01f))), 1.0f);
    dout.texcoord = texcoord;
    dout.parameter = parameter;
    
    return dout;
}
