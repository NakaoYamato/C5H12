#include "TerrainShadow.hlsli"
#include "../../Define/SamplerStateDefine.hlsli"
SamplerState samplerStates[_SAMPLER_STATE_MAX] : register(s0);

// パラメータマップ
Texture2D<float4> parameterTexture : register(t2);

[domain("quad")]
CSM_DS_OUT main(HS_CONSTANT_OUT input,
float2 UV : SV_DomainLocation,
const OutputPatch<CSM_DS_IN, 4> patch)
{
    float u = UV.x;
    float v = UV.y;
    float uBar = 1.0f - u;
    float vBar = 1.0f - v;
    
    float uv = u * v;
    float uBarv = uBar * v;
    float uvBar = u * vBar;
    float uBarvBar = uBar * vBar;
    
    CSM_DS_OUT dout = (CSM_DS_OUT) 0;
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
    float height = parameter.r + parameter.b * texHeightInfluence;
    
    // 座標の適用
    worldPosition.y += height;
    
    // オフセットを足す
    worldPosition.y += shadowOffset;

    // 情報設定
    dout.position = mul(float4(worldPosition, 1.0), csm_data.cascaded_matrices[patch[0].instanceId]);
    dout.instanceId = patch[0].instanceId;
    
    return dout;
}
