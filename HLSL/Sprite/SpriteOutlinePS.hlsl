#include "Sprite.hlsli"

Texture2D colorMap : register(t0);
#include "../Define/SamplerStateDefine.hlsli"
SamplerState samplerStates[_SAMPLER_STATE_MAX] : register(s0);

cbuffer CBOutline : register(b2)
{
    float4 outlineColor;
    float2 outlineScale;
    float intensity;
    float Padding;
}

float4 main(VsOut pin) : SV_TARGET
{
    // テクスチャのサイズ取得
    float2 colorMapSize;
    colorMap.GetDimensions(colorMapSize.x, colorMapSize.y);
    
    float4 centerColor = colorMap.Sample(samplerStates[_POINT_WRAP_SAMPLER_INDEX], pin.texcoord);
    
    float totalAlpha = 0.0f;   
    // オフセット配列（8方向）
    float2 offsets[8] =
    {
        float2(-1, -1), float2(0, -1),  float2(1, -1),
        float2(-1, 0),                  float2(1, 0),
        float2(-1, 1),  float2(0, 1),   float2(1, 1)
    };

    [unroll]
    for (int i = 0; i < 8; i++)
    {
        // ピクセル単位のズレを作る
        float2 uv = pin.texcoord + offsets[i] * outlineScale / colorMapSize;
        totalAlpha += colorMap.Sample(samplerStates[_POINT_WRAP_SAMPLER_INDEX], uv).a;
    }
    
    float outlineAlpha = saturate((totalAlpha / 8.0f) * intensity);
    
    float4 outColor = outlineColor;
    outColor.a *= outlineAlpha;
    float4 bodyColor = pin.color * centerColor.a;
    
    float4 finalColor = lerp(outColor, bodyColor, centerColor.a);
    
    finalColor.a = max(centerColor.a, outColor.a);
    
    clip(finalColor.a - 0.001f);

    return finalColor;
}