#include "TerrainDeform.hlsli"
#include "../../Function/Noise.hlsli"

PS_OUT main(VsOut pin)
{
    float4 baseColor, baseNormal, baseParameter;
    SampleBaseTexture(pin.texcoord, baseColor, baseNormal, baseParameter);
    float rate = CalculateBrushRate(pin.texcoord) * brushStrength;
    
    float noise = Noise(pin.texcoord * padding.x * 100.0f) * padding.y;
    baseParameter.r = lerp(baseParameter.r, noise, rate);
    
    PS_OUT pout = (PS_OUT) 0;
    pout.color = baseColor;
    pout.normal = baseNormal;
    pout.parameter = baseParameter;
    return pout;
}