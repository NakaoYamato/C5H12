#include "../../Sprite/Sprite.hlsli"

cbuffer CHROMATIC_ABERRATION_BUFFER : register(b1)
{
    float chromaticAberrationAmount;
    uint chromaticAberrationMaxSamples;
    float2 chromaticAberrationDummy;

    float4 chromaticAberrationShift[3];
};

Texture2D texture0 : register(t0);
SamplerState sampler0 : register(s0);

float4 main(VsOut pin) : SV_TARGET
{
    float2 scene_map_size;
    texture0.GetDimensions(scene_map_size.x, scene_map_size.y);
    float2 scene_map_texel_size = (float2) 1.0f / scene_map_size;
    
    float2 coords = 2.0f * pin.texcoord - 1.0f;
    float2 end = pin.texcoord - coords * dot(coords, coords) * chromaticAberrationAmount;
    
    float2 diff = end - pin.texcoord;
    int samples = clamp(int(length(scene_map_texel_size * diff / 2.0f)), 3, chromaticAberrationMaxSamples);
    float2 delta = diff / samples;
    float2 pos = pin.texcoord;
    float4 sum = (0.0f).xxxx, filterSum = (0.0f).xxxx;
    for (uint index = 0; index < chromaticAberrationMaxSamples; ++index)
    {
        uint t = (uint) (((float) index + 0.5f) / samples);
        float4 s = texture0.Sample(sampler0, pos);
        float3 rgb = chromaticAberrationShift[t % 3].rgb;
        float4 filter = float4(rgb, 1.0f);
        
        sum += s * filter;
        filterSum += filter;
        pos += delta;
    }

    return sum / filterSum;
}