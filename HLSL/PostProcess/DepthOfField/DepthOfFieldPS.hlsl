#include "../../Sprite/Sprite.hlsli"

cbuffer DEPTH_OF_FIELD_CONSTANT_BUFFER : register(b1)
{
    row_major float4x4 view;
    row_major float4x4 invViewProjection;
    
    float focusDistance;
    float dofRange;
    float2 glowDummy;
};

#include "../../Define/SamplerStateDefine.hlsli"
SamplerState samplerStates[_SAMPLER_STATE_MAX] : register(s0);

Texture2D colorMap : register(t0);
Texture2D depthMap : register(t1);
Texture2D gradationMap : register(t2);

float4 main(VsOut pin) : SV_TARGET
{
    // グローバル座標取得
    float depth = depthMap.Sample(samplerStates[_POINT_WRAP_SAMPLER_INDEX], pin.texcoord).x;
    float4 position = float4(pin.texcoord.x * 2.0f - 1.0f, pin.texcoord.y * -2.0f + 1.0f, depth, 1);
    position = mul(position, invViewProjection);
    float4 worldPosition = float4(position.xyz / position.w, 1.0f);
    
    //  焦点距離と焦点範囲からブレンド係数を算出
    float wDepth = mul(worldPosition, view).z;
    float alpha = abs(wDepth - focusDistance) / dofRange;
    alpha = saturate(alpha);
    
    //  ぼかし色取得
    float3 bokeh = gradationMap.Sample(samplerStates[_LINEAR_WRAP_SAMPLER_INDEX], pin.texcoord).rgb;
    
    float4 color = colorMap.Sample(samplerStates[_LINEAR_WRAP_SAMPLER_INDEX], pin.texcoord);
    //  合成
    color.rgb = bokeh * (1.0 - alpha) + color.rgb * (alpha);
    color.a = 1.0f;
    return color;
}