#include "../CBuffer/B0/SceneCB.hlsli"
#include "../CBuffer/B2/DecalCB.hlsli"

struct PS_OUT
{
    float4 baseColor : SV_TARGET0;
    float4 worldNormal : SV_TARGET1;
};

struct DecalDecodeData
{
    float2 gbufferTexcoord;
    float2 cubeTexcoord;
    float depth;
    float3 worldPosition;
};

inline DecalDecodeData GetDecalTexcoord(float2 svPosition,
Texture2D<float> gbufferDepth, 
SamplerState samplerState, 
row_major float4x4 invViewProjection,
row_major float4x4 invWorld)
{
    DecalDecodeData res;
    
    // スクリーン情報からGBuffer参照用UV座標を算出
    float2 screenSize;
    gbufferDepth.GetDimensions(screenSize.x, screenSize.y);
    res.gbufferTexcoord = svPosition.xy / screenSize;
    
    // 深度値からワールド座標を算出
    res.depth = gbufferDepth.Sample(samplerState, res.gbufferTexcoord).x;
    float4 position = float4(res.gbufferTexcoord.x * 2.0f - 1.0f, res.gbufferTexcoord.y * -2.0f + 1.0f, res.depth, 1);
    position = mul(position, invViewProjection);
    res.worldPosition = position.xyz / position.w;
    
    // キューブ基準の空間の座標に変換してデカールのUV座標に変換
    float4 cubeTexturePosition = mul(float4(res.worldPosition, 1), invWorld);
    cubeTexturePosition /= cubeTexturePosition.w;
    clip(abs(cubeTexturePosition.x) > 1.0f ? -1 : 1);
    clip(abs(cubeTexturePosition.y) > 1.0f ? -1 : 1);
    clip(abs(cubeTexturePosition.z) > 0.5f ? -1 : 1);
    cubeTexturePosition.x = cubeTexturePosition.x * 0.5f + 0.5f;
    cubeTexturePosition.y = cubeTexturePosition.y * -0.5f + 0.5f;
    res.cubeTexcoord = cubeTexturePosition.xy;
    return res;
}

inline void DecalMask(float2 gbufferTexcoord,
Texture2D<float4> gbufferParameter,
SamplerState samplerState,
int mask)
{
    if (mask == 0)
    {
        return;
    }
    
    float4 parameter = gbufferParameter.Sample(samplerState, gbufferTexcoord);
    if (mask != (int)parameter.x)
        discard;
}