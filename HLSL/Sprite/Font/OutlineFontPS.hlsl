// SpriteBatchからの入力
struct PS_IN
{
    float4 color : COLOR0;          // DrawStringで指定した文字色
    float2 texCoord : TEXCOORD0;    // UV座標
};

cbuffer CBOutline : register(b2)
{
    float4 outlineColor;
    float2 outlineScale;
    float intensity;
    float Padding;
}

Texture2D<float4> Texture0 : register(t0); // フォントテクスチャ
#include "../../Define/SamplerStateDefine.hlsli"
SamplerState samplerStates[_SAMPLER_STATE_MAX] : register(s0);

float4 main(PS_IN pin) : SV_TARGET
{
    // テクスチャのサイズ取得
    float2 texture0Size;
    Texture0.GetDimensions(texture0Size.x, texture0Size.y);
    
    float4 centerColor = Texture0.Sample(samplerStates[_POINT_WRAP_SAMPLER_INDEX], pin.texCoord);
    
    float totalAlpha = 0.0f;
    // オフセット配列（8方向）
    float2 offsets[8] =
    {
        float2(-1, -1), float2(0, -1), float2(1, -1),
        float2(-1, 0), float2(1, 0),
        float2(-1, 1), float2(0, 1), float2(1, 1)
    };

    [unroll]
    for (int i = 0; i < 8; i++)
    {
        // ピクセル単位のズレを作る
        float2 uv = pin.texCoord + offsets[i] * outlineScale / texture0Size;
        totalAlpha += Texture0.Sample(samplerStates[_POINT_WRAP_SAMPLER_INDEX], uv).a;
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