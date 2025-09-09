#include "TerrainDeform.hlsli"

PS_OUT main(VsOut pin)
{
    float4 baseColor, baseNormal, baseParameter;
    SampleBaseTexture(pin.texcoord, baseColor, baseNormal, baseParameter);
    float rate = CalculateBrushRate(pin.texcoord) * brushStrength;
    
    if (padding.x > 0.0f)
    {
        if (baseParameter.r < padding.y)
            baseParameter.r = lerp(baseParameter.r, padding.y, rate);
    }
    else
    {
        if (baseParameter.r > padding.y)
            baseParameter.r = lerp(baseParameter.r, padding.y, rate);
    }
    
    PS_OUT pout = (PS_OUT) 0;
    pout.color = baseColor;
    pout.normal = baseNormal;
    pout.parameter = baseParameter;
    return pout;
}