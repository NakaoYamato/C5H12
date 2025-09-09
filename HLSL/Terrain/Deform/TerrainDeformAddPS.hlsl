#include "TerrainDeform.hlsli"

PS_OUT main(VsOut pin)
{
    float4 baseColor, baseNormal, baseParameter;
    SampleBaseTexture(pin.texcoord, baseColor, baseNormal, baseParameter);
    float rate = CalculateBrushRate(pin.texcoord);
    
    if (rate <= 0.0f)
        discard;

    float2 paintTexcoord = pin.texcoord * textureTillingScale;
    float4 color = paintColorTexture.Sample(samplerStates[_POINT_WRAP_SAMPLER_INDEX], paintTexcoord);
    color = lerp(baseColor, color, brushStrength * rate);
    color = saturate(color);
    
    float4 normal = paintNormalTexture.Sample(samplerStates[_POINT_WRAP_SAMPLER_INDEX], paintTexcoord);
    normal = lerp(baseNormal, normal, brushStrength * rate);
    normal = normalize(normal);
    
    float4 height = paintHeightTexture.Sample(samplerStates[_POINT_WRAP_SAMPLER_INDEX], paintTexcoord);
    baseParameter.b = lerp(baseParameter.b, height.r, brushStrength * rate);
    
    PS_OUT pout = (PS_OUT) 0;
    pout.color = color;
    pout.normal = normal;
    pout.parameter = baseParameter;
    return pout;
}