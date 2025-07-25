#include "Terrain.hlsli"
#include "../Define/SamplerStateDefine.hlsli"
SamplerState samplerStates[_SAMPLER_STATE_MAX] : register(s0);

// パラメータマップ
Texture2D<float4> parameterTexture : register(t6);

[domain("tri")]
DS_OUT main(HS_CONSTANT_OUT input, float3 UV : SV_DomainLocation,
const OutputPatch<DS_IN, 3> patch)
{
    DS_OUT dout = (DS_OUT) 0;
    // 頂点UV座標
    float2 texcoord = patch[0].texcoord * UV.x + patch[1].texcoord * UV.y + patch[2].texcoord * UV.z;
    // 頂点座標
    float3 position = patch[0].position.xyz * UV.x + patch[1].position.xyz * UV.y + patch[2].position.xyz * UV.z;
    // 頂点法線
    float3 normal = normalize(patch[0].normal * UV.x + patch[1].normal * UV.y + patch[2].normal * UV.z);
    // ワールド法線
    float3 worldNormal = normalize(mul(normal, (float3x3) world));
    // パラメータマップから高さ方向取得
    float4 parameter = parameterTexture.SampleLevel(samplerStates[_POINT_CLAMP_SAMPLER_INDEX], texcoord, 0);
    float height = parameter.r * heightSclaer;
    // 頂点座標をハイトマップで取得した値分ずらす
    {
        position = mul(float4(position, 1.0f), world).xyz;
        position += worldNormal * height;
    }
    // 情報設定
    dout.position = mul(float4(position, 1.0), viewProjection);
    dout.worldPosition = position;
    dout.worldNormal = worldNormal;
    dout.worldTangent = float4(normalize(cross(worldNormal, float3(0.0f, 1.0f, 0.01f))), 1.0f);
    dout.texcoord = texcoord;
    
    // コストはストリームアウト時に計算するので初期化
    dout.cost = 0.0f;
    
    return dout;
}
