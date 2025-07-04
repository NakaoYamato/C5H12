#include "TerrainDeform.hlsli"

// マテリアルマップに書き込むので出力先は2つ
PS_OUT main(VsOut pin)
{
    // ブラシの影響度を受けるか判定
    float len = length(pin.texcoord - brushUVPosition);
    clip(brushRadius - len);

    // 影響割合を計算
    float rate = saturate(len / brushRadius);
    
    float2 brushTexcoord = pin.texcoord * textureTillingScale;
    float4 color = brushColorTexture.Sample(samplerStates[_POINT_WRAP_SAMPLER_INDEX], brushTexcoord);
    float4 baseColor = colorTexture.Sample(samplerStates[_POINT_WRAP_SAMPLER_INDEX], pin.texcoord);
    color = lerp(baseColor, color, brushStrength * rate);
    color = saturate(color);
    float4 normal = brushNormalTexture.Sample(samplerStates[_POINT_WRAP_SAMPLER_INDEX], brushTexcoord);
    float4 baseNormal = normalTexture.Sample(samplerStates[_POINT_WRAP_SAMPLER_INDEX], pin.texcoord);
    normal = lerp(baseNormal, normal, brushStrength * rate);
    normal = normalize(normal);
    
    PS_OUT pout = (PS_OUT) 0;
    pout.color = color;
    pout.normal = normal;
    return pout;
}