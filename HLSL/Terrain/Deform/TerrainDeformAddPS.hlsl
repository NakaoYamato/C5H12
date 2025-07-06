#include "TerrainDeform.hlsli"

// マテリアルマップに書き込むので出力先は2つ
PS_OUT main(VsOut pin)
{
    float rate = CalculateBrushRate(pin.texcoord);

    float2 paintTexcoord = pin.texcoord * textureTillingScale;
    float4 color = paintColorTexture.Sample(samplerStates[_POINT_WRAP_SAMPLER_INDEX], paintTexcoord);
    float4 baseColor = colorTexture.Sample(samplerStates[_POINT_WRAP_SAMPLER_INDEX], pin.texcoord);
    color = lerp(baseColor, color, brushStrength * rate);
    color = saturate(color);
    float4 normal = paintNormalTexture.Sample(samplerStates[_POINT_WRAP_SAMPLER_INDEX], paintTexcoord);
    float4 baseNormal = normalTexture.Sample(samplerStates[_POINT_WRAP_SAMPLER_INDEX], pin.texcoord);
    normal = lerp(baseNormal, normal, brushStrength * rate);
    normal = normalize(normal);
    
    PS_OUT pout = (PS_OUT) 0;
    pout.color = color;
    pout.normal = normal;
    return pout;
}