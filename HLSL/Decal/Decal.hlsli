#include "../Scene/SceneConstantBuffer.hlsli"

struct PS_OUT
{
    float4 baseColor : SV_TARGET0;
    float4 worldNormal : SV_TARGET1;
};

cbuffer GBUFFER_DECAL_CONSTANT_BUFFER : register(b2)
{
    row_major float4x4 world;
    row_major float4x4 invWorld;
    float4 materialColor;
    float4 decalDirection;
};

inline float2 GetDecalTexcoord(float2 svPosition, 
Texture2D<float> gbufferDepth, 
SamplerState samplerState, 
row_major float4x4 invViewProjection,
row_major float4x4 invWorld)
{
    // スクリーン情報からGBuffer参照用UV座標を算出
    float2 screenSize;
    gbufferDepth.GetDimensions(screenSize.x, screenSize.y);
    float2 gbufferTexcoord = svPosition.xy / screenSize;
    
    // 深度値からワールド座標を算出
    float depth = gbufferDepth.Sample(samplerState, gbufferTexcoord).x;
    float4 position = float4(gbufferTexcoord.x * 2.0f - 1.0f, gbufferTexcoord.y * -2.0f + 1.0f, depth, 1);
    position = mul(position, invViewProjection);
    float3 worldPosition = position.xyz / position.w;
    
    // キューブ基準の空間の座標に変換してデカールのUV座標に変換
    float4 cubeTexturePosition = mul(float4(worldPosition, 1), invWorld);
    cubeTexturePosition /= cubeTexturePosition.w;
    cubeTexturePosition.x = cubeTexturePosition.x * 0.5f + 0.5f;
    cubeTexturePosition.y = cubeTexturePosition.y * -0.5f + 0.5f;
    return cubeTexturePosition.xy;    
}