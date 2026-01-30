#include "Sprite.hlsli"

Texture2D colorMap : register(t0);
#include "../Define/SamplerStateDefine.hlsli"
SamplerState samplerStates[_SAMPLER_STATE_MAX] : register(s0);

Texture2D maskMap : register(t10);

cbuffer CBDissolve : register(b2)
{
    float amount;
    float borderWidth;
    float2 maskScroll;
    float4 borderColor;
}

float4 main(VsOut pin) : SV_TARGET
{
    float4 color = colorMap.Sample(samplerStates[_POINT_WRAP_SAMPLER_INDEX], pin.texcoord);
    float4 mask = maskMap.Sample(samplerStates[_POINT_WRAP_SAMPLER_INDEX], pin.texcoord + maskScroll);
    
    // 透明部分は描画しない)
    clip(color.a - 0.01);
    
    // mask.r が amount 未満なら描画しない
    clip(mask.r - amount);
    
    float isBorder = step(mask.r, amount + borderWidth);

    // isBorderが1ならボーダー色、0なら元の色を使う
    if (isBorder > 0.0)
    {
        color = borderColor;        
    }

    return color;
}