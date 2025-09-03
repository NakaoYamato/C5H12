#include "TerrainDeform.hlsli"

// パラメータマップに書き込むので出力先は1つ
float4 main(VsOut pin) : SV_TARGET
{
    float rate = CalculateBrushRate(pin.texcoord) * brushStrength;

    float4 parameter = parameterTexture.SampleLevel(samplerStates[_POINT_WRAP_SAMPLER_INDEX], pin.texcoord, 0);
    if (padding.x > 0.0f)
    {
        if (parameter.r < padding.y)
            parameter.r = lerp(parameter.r, padding.y, rate);
    }
    else
    {
        if (parameter.r > padding.y)
            parameter.r = lerp(parameter.r, padding.y, rate);
    }
    return parameter;
}