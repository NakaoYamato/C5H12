#include "TerrainDeform.hlsli"

// パラメータマップ
Texture2D<float4> parameterTexture : register(t6);

float4 main(VsOut pin) : SV_TARGET
{
    // ブラシの影響度を受けるか判定
    float len = length(pin.texcoord - brushUVPosition);
    clip(brushRadius - len);

    // 影響割合を計算
    float rate = 1.0f - saturate(len / brushRadius);

    float3 color = brushColor.rgb;
    float4 baseColor = texture0.Sample(samplerStates[_POINT_WRAP_SAMPLER_INDEX], pin.texcoord);
    color = lerp(baseColor.rgb, color, brushStrength * rate);
    color = saturate(color);
    return float4(color, baseColor.a);
}