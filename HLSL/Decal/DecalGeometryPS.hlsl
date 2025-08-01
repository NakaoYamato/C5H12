#include "DecalGeometry.hlsli"
#include "../Define/SamplerStateDefine.hlsli"
SamplerState samplerStates[_SAMPLER_STATE_MAX] : register(s0);

//  デカールテクスチャ
Texture2D<float4> decale_texture : register(t0);
Texture2D<float4> decaleNormalTexture : register(t1);

Texture2D<float> gbufferDepth : register(t2);

PS_OUT main(VS_OUT pin)
{
    // スクリーン情報からGBuffer参照用UV座標を算出
    float2 screen_size;
    gbufferDepth.GetDimensions(screen_size.x, screen_size.y);
    float2 gbufferTexcoord = pin.position.xy / screen_size;
    
    // 深度値からワールド座標を算出
    float depth = gbufferDepth.Sample(samplerStates[_POINT_WRAP_SAMPLER_INDEX], gbufferTexcoord).x;
    float4 position = float4(gbufferTexcoord.x * 2.0f - 1.0f, gbufferTexcoord.y * -2.0f + 1.0f, depth, 1);
    position = mul(position, invViewProjection);
    float3 worldPosition = position.xyz / position.w;
    
    // キューブ基準の空間の座標に変換してデカールのUV座標に変換
    float4 cubeTexturePosition = mul(float4(worldPosition, 1), decal_inverse_transform);
    cubeTexturePosition /= cubeTexturePosition.w;
    cubeTexturePosition.x = cubeTexturePosition.x * 0.5f + 0.5f;
    cubeTexturePosition.y = cubeTexturePosition.y * -0.5f + 0.5f;
    
    float2 decalTexcoord = cubeTexturePosition.xy;
    float4 color = decale_texture.Sample(samplerStates[_LINEAR_BORDER_BLACK_INDEX], decalTexcoord) * pin.color;
    clip(color.a < 0.5f ? -1 : 1);
    
    float4 normal = decaleNormalTexture.Sample(samplerStates[_LINEAR_BORDER_BLACK_INDEX], decalTexcoord);
    normal = normalize(mul(float4((normal * 2.0f - 1.0f).xyz, 0.0f), world));
    
    PS_OUT pout;
    pout.baseColor = color;
    pout.baseColor.a = 0.5f;
    pout.worldNormal = float4(normal.xyz, 0.5f);
    return pout;
}