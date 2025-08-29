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

    // --- 双線形補間 (Bilinear Interpolation) ---
    // C++側のインデックス順序 (v0, v1, v3, v2) に対応
    // patch[0]: 左上 (u=0, v=0)
    // patch[1]: 左下 (u=0, v=1)
    // patch[2]: 右下 (u=1, v=1)
    // patch[3]: 右上 (u=1, v=0)

    // 上の辺 (左上 -> 右上) をuで補間
    DS_IN top = (DS_IN) 0;
    top.position = lerp(patch[0].position, patch[3].position, u);
    top.texcoord = lerp(patch[0].texcoord, patch[3].texcoord, u);
    top.normal = lerp(patch[0].normal, patch[3].normal, u);

    // 下の辺 (左下 -> 右下) をuで補間
    DS_IN bottom = (DS_IN) 0;
    bottom.position = lerp(patch[1].position, patch[2].position, u);
    bottom.texcoord = lerp(patch[1].texcoord, patch[2].texcoord, u);
    bottom.normal = lerp(patch[1].normal, patch[2].normal, u);

    // 上の辺と下の辺の結果をvで補間
    DS_IN finalAttrib = (DS_IN) 0;
    finalAttrib.position = lerp(top.position, bottom.position, v);
    finalAttrib.texcoord = lerp(top.texcoord, bottom.texcoord, v);
    finalAttrib.normal = lerp(top.normal, bottom.normal, v);
    // -----------------------------------------

    DS_OUT dout = (DS_OUT) 0;
    float2 texcoord = finalAttrib.texcoord;
    float3 position = finalAttrib.position.xyz;
    float3 normal = normalize(finalAttrib.normal);
    
    // ワールド法線
    float3 worldNormal = normalize(mul(normal, (float3x3) world));
    // パラメータマップから高さ方向取得
    float4 parameter = parameterTexture.SampleLevel(samplerStates[_POINT_CLAMP_SAMPLER_INDEX], texcoord, 0);
    float height = parameter.r;
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
    dout.parameter = parameter;
    
    return dout;
}
