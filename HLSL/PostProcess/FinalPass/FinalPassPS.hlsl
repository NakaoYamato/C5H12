#include "../../Sprite/Sprite.hlsli"
#include "FilterFunctions.hlsli"

cbuffer CONSTANT_BUFFER : register(b1)
{
    // カラーフィルター
    float hueShift; // 色相調整
    float saturation; // 彩度調整
    float brightness; // 明度調整
    float cfDummy;
    
    // トーンマッピング
    float exposure; // 補正値
    int toneType; // 使用するトーンタイプ
    float2 tmDummy;
    
    // ヴィネット
    float4 vignetteColor;

    float2 vignetteCenter;
    float vignetteIntensity;
    float vignetteSmoothness;

    int vignetteRounded; // 1 : use  0 : unUse
    float vignetteRoundness;
    float vignetteDummy[2];
};

Texture2D texture0 : register(t0);
SamplerState sampler0 : register(s0);

float4 main(VsOut pin) : SV_TARGET
{
    float4 color = texture0.Sample(sampler0, pin.texcoord);
    
    // カラーフィルター
#if 1
	// RGB > HSVに変換
    color.rgb = RGB2HSV(color.rgb);

	// 色相調整
    color.r += hueShift;

	// 彩度調整
    color.g *= saturation;

	// 明度調整
    color.b *= brightness;

	// HSV > RGBに変換
    color.rgb = HSV2RGB(color.rgb);
#endif
        
    // トーンマッピング
#if 1
    if (toneType == 0)
    {
        color.rgb *= 1.0 - exp(-color.rgb * exposure);
    }
    if (toneType == 1)
    {
        color *= exp2(exposure);
        color /= 1.0 + color;
    }
    if (toneType == 2)
    {
        color *= exp2(exposure);
        color = max(color - 0.004, 0.0);
        color = (color * (6.2 * color + 0.5)) / (color * (6.2 * color + 1.7) + 0.06);
    }
#endif
    
#if 1
	// Gamma process
    const float GAMMA = 2.2;
    color = pow(color, 1.0 / GAMMA);
#endif
    
    // ヴィネット
#if 1
    float2 scene_map_size;
    texture0.GetDimensions(scene_map_size.x, scene_map_size.y);
    // 周辺減光処理
    float2 d = abs(pin.texcoord - vignetteCenter) * (vignetteIntensity);
    // 減光をスクリーンに合わすかどうか
    d.x *= lerp(1.0f, scene_map_size.x / scene_map_size.y, (float) vignetteRounded);
    // 隅の濃さ
    d = pow(saturate(d), vignetteRoundness);
    half vignette_factor = pow(saturate(1.0f - dot(d, d)), vignetteSmoothness);
    color.rgb *= lerp(vignetteColor.rgb, (float3) 1.0f, vignette_factor);
#endif
    
    return color;
}