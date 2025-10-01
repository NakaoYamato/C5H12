#include "PrimitiveRenderer.hlsli"

#include "../Define/SamplerStateDefine.hlsli"
SamplerState samplerStates[_SAMPLER_STATE_MAX] : register(s0);

Texture2D<float4> gbufferColor : register(t2);
Texture2D distanceMap : register(t3);
Texture2D noiseMap : register(t4);

float4 main(VS_OUT pin) : SV_TARGET
{
    {
        float4 distance = distanceMap.Sample(samplerStates[_POINT_WRAP_SAMPLER_INDEX], pin.texcoord);
        float rate = saturate(length(distance.xyz));
        if (rate > 0.5f && rate < 0.7f)
            return float4(1.0f, 1.0f, 1.0f, 1.0f) * pin.texcoord.y;
        //return float4(rate, 0.0f, 0.0f, 1.0f);
        float2 vec = (pin.texcoord - float2(0.5f, 0.5f)) * 0.3f * rate;
        float2 screenUV = pin.position.xy / viewportSize;
        //float2 uv = screenUV + (distance.xy * 2.0f - float2(1.0f, 1.0f));
        float4 color = gbufferColor.Sample(samplerStates[_POINT_WRAP_SAMPLER_INDEX], screenUV + vec);
        return color;
    }
    
    
    {
    //return float4(pin.texcoord.x, pin.texcoord.y, 0.0f, 1.0f);
    
        float rate = saturate(length(pin.texcoord - float2(0.5f, 0.5f)));
    
        float2 noiseTexcoord = pin.texcoord;
    //noiseTexcoord.x *= 0.0001f;
        noiseTexcoord.y *= 0.2f;
        float noise = noiseMap.Sample(samplerStates[_POINT_WRAP_SAMPLER_INDEX], noiseTexcoord).r;
    
        float2 distanceTexcoord = pin.texcoord;
    //distanceTexcoord.x *= 10.0f;
    //distanceTexcoord.x -= totalElapsedTime * 0.01f;
    //distanceTexcoord.y = 0.0f;
        float4 distance = distanceMap.Sample(samplerStates[_POINT_WRAP_SAMPLER_INDEX], distanceTexcoord);
    
    //return float4(distance.xyz, 1.0f);
    
        distance.x *= 0.1f;
    //distance.y *= 0.01f;
        distance.y = 0.0f;
        float2 screenUV = pin.position.xy / viewportSize;
        float2 uv = screenUV + (distance.xy * 2.0f - float2(1.0f, 1.0f));
        float4 color = gbufferColor.Sample(samplerStates[_POINT_WRAP_SAMPLER_INDEX], frac(uv));
    //float alpha = noise < (rate / 2.0f) ? 0.0f : 1.0f;
    //if (rate > 0.25f)
    //    alpha = 0.0f;
    
    //return gbufferColor.Sample(samplerStates[_POINT_WRAP_SAMPLER_INDEX], screenUV);
    
    //return float4(pin.color.rgb * noise + color.rgb * (1.0f - noise), pow(rate * 2.0f, 2.0f));
        return float4(pin.color.rgb * noise + color.rgb * (1.0f - noise), 1.0f);
    }
}
