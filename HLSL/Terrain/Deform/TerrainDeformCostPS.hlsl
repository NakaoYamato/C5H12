#include "TerrainDeform.hlsli"

// パラメータマップに書き込むので出力先は1つ
float4 main(VsOut pin) : SV_TARGET
{
    // ブラシの影響度を受けるか判定
    float len = length(pin.texcoord - brushUVPosition);
    clip(brushRadius - len);

    // 影響割合を計算
    float rate = 1.0f - saturate(len / brushRadius);
    
    float4 parameter = parameterTexture.SampleLevel(samplerStates[_POINT_WRAP_SAMPLER_INDEX], pin.texcoord, 0);
    parameter.rg = clamp(parameter.g + brushStrength * rate, heightScale.x, heightScale.y);
    return parameter;
}