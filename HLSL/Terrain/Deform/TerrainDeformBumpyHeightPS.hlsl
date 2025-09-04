#include "TerrainDeform.hlsli"
#include "../../Function/Noise.hlsli"

// パラメータマップに書き込むので出力先は1つ
float4 main(VsOut pin) : SV_TARGET
{
    float rate = CalculateBrushRate(pin.texcoord) * brushStrength;

    float4 parameter = parameterTexture.SampleLevel(samplerStates[_POINT_WRAP_SAMPLER_INDEX], pin.texcoord, 0);
    
    float noise = Noise(pin.texcoord * padding.x * 100.0f) * padding.y;
    parameter.r = lerp(parameter.r, noise, rate);
    return parameter;
}