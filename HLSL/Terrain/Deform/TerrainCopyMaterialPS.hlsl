#include "TerrainDeform.hlsli"

PS_OUT main(VsOut pin)
{
    PS_OUT pout = (PS_OUT) 0;
    float4 baseColor, baseNormal, baseParameter;
    SampleBaseTexture(pin.texcoord, baseColor, baseNormal, baseParameter);
    pout.color = baseColor;
    pout.normal = baseNormal;
    pout.parameter = baseParameter;
    return pout;
}