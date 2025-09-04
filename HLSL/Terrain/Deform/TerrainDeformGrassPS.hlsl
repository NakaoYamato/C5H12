#include "TerrainDeform.hlsli"

// パラメータマップに書き込むので出力先は1つ
float4 main(VsOut pin) : SV_TARGET
{
    float rate = CalculateBrushRate(pin.texcoord);

    float factor = brushStrength * rate;
    float4 parameter = parameterTexture.SampleLevel(samplerStates[_POINT_WRAP_SAMPLER_INDEX], pin.texcoord, 0);
    parameter.g = max(parameter.g, factor);
    return parameter;
}