#include "../../Sprite/Sprite.hlsli"

cbuffer TONEMAPPING_BUFFER : register(b1)
{
    float exposure; // 補正値
    int toneType;   // 使用するトーンタイプ
    float2 dummy;
};

#include "../../Function/ShadingFunctions.hlsli"

Texture2D texture0 : register(t0);
SamplerState sampler0 : register(s0);

float4 main(VsOut pin) : SV_TARGET
{
    float4 color = texture0.Sample(sampler0, pin.texcoord);
    
    // トーンマッピング
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
    
#if 1
	// Gamma process
    const float GAMMA = 2.2;
    color = pow(color, 1.0 / GAMMA);
#endif
    
    return color;
}