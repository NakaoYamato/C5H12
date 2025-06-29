#include "Terrain.hlsli"
#include "../Define/SamplerStateDefine.hlsli"
SamplerState samplerStates[_SAMPLER_STATE_MAX] : register(s0);

// ハイトマップ
Texture2D<float4> heightTextures : register(t5);

[domain("tri")]
DS_OUT main(HS_CONSTANT_OUT input, float3 UV : SV_DomainLocation,
const OutputPatch<DS_IN, 3> patch)
{
    DS_OUT dout = (DS_OUT) 0;
    // 頂点色
    float4 color = patch[0].color * UV.x + patch[1].color * UV.y + patch[2].color * UV.z;
    // 頂点UV座標
    float2 texcoord = patch[0].texcoord * UV.x + patch[1].texcoord * UV.y + patch[2].texcoord * UV.z;
    // 頂点座標
    float3 position = patch[0].position.xyz * UV.x + patch[1].position.xyz * UV.y + patch[2].position.xyz * UV.z;
    // 頂点法線
    float3 normal = normalize(patch[0].normal * UV.x + patch[1].normal * UV.y + patch[2].normal * UV.z);
    // ワールド法線
    float3 world_normal = normalize(mul(normal, (float3x3) world));
    // ハイトマップ
    float4 weight_textures = heightTextures.SampleLevel(samplerStates[_POINT_CLAMP_SAMPLER_INDEX], texcoord, 0);
    float height = weight_textures.x * heightSclaer;
    // 頂点座標をハイトマップで取得した値分ずらす
    {
        position = mul(float4(position, 1.0f), world).xyz;
        position += world_normal * height;
    }
    // 情報設定
    dout.position = mul(float4(position, 1.0), viewProjection);
    dout.texcoord = texcoord;
    dout.normal = world_normal;
    dout.color = color;
    
    dout.worldPosition = position;
    dout.grassWeight = weight_textures.b;
    
    return dout;
}
