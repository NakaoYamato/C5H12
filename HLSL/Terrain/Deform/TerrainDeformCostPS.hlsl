#include "TerrainDeform.hlsli"

PS_OUT main(VsOut pin)
{
    float4 baseColor, baseNormal, baseParameter;
    SampleBaseTexture(pin.texcoord, baseColor, baseNormal, baseParameter);
    float rate = CalculateBrushRate(pin.texcoord);
    
    baseParameter.b = baseParameter.b + brushStrength * rate;
    
    PS_OUT pout = (PS_OUT) 0;
    pout.color = baseColor;
    pout.normal = baseNormal;
    pout.parameter = baseParameter;
    return pout;
}