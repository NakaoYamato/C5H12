#include "TerrainShadow.hlsli"
#include "../../Define/SamplerStateDefine.hlsli"
SamplerState samplerStates[_SAMPLER_STATE_MAX] : register(s0);

// パラメータマップ
Texture2D<float4> parameterTexture : register(t2);

[domain("quad")]
DS_OUT_CSM main(HS_CONSTANT_OUT input,
float2 UV : SV_DomainLocation,
const OutputPatch<DS_IN_CSM, 4> patch)
{
    float u = UV.x;
    float v = UV.y;
    float uBar = 1.0f - u;
    float vBar = 1.0f - v;
    
    float uv = u * v;
    float uBarv = uBar * v;
    float uvBar = u * vBar;
    float uBarvBar = uBar * vBar;
    
    DS_OUT_CSM dout = (DS_OUT_CSM) 0;
    float3 position =
    patch[0].position * uBarvBar +
    patch[1].position * uBarv +
    patch[2].position * uv +
    patch[3].position * uvBar;
    float3 worldPosition = mul(float4(position, 1.0f), world).xyz;
    // positionをUVに変換
    float2 texcoord = float2(position.x / terrainLength, 1.0f - position.z / terrainLength);
    
    // パラメータマップから高さ方向取得
    float4 parameter = parameterTexture.SampleLevel(samplerStates[_POINT_CLAMP_SAMPLER_INDEX], texcoord, 0);
    float height = parameter.r + parameter.b;
    // 頂点座標をハイトマップで取得した値分ずらす
    worldPosition.y += height;
    // 情報設定
    dout.position = mul(float4(worldPosition, 1.0), csm_data.cascaded_matrices[patch[0].instanceId]);
    dout.instanceId = patch[0].instanceId;
    
    return dout;
}
