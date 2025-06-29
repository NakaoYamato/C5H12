#include "Terrain.hlsli"
#include "../Define/SamplerStateDefine.hlsli"
SamplerState samplerStates[_SAMPLER_STATE_MAX] : register(s0);

#define ROCK_COLOR 0
#define DIRT_COLOR 1
#define GRASS_COLOR 2
Texture2D terrainTextures[3] : register(t0);
// ハイトマップ
Texture2D<float4> heightTextures : register(t5);

float4 main(PS_IN pin) : SV_TARGET
{
    // ブレンドしたいテクスチャの色を取得
    float2 tilling_coord = pin.texcoord * tillingScale;
    float4 rock_color = terrainTextures[ROCK_COLOR].Sample(samplerStates[_ANISOTROPIC_SAMPLER_INDEX], tilling_coord);
    float4 dirt_color = terrainTextures[DIRT_COLOR].Sample(samplerStates[_ANISOTROPIC_SAMPLER_INDEX], tilling_coord);
    float4 grass_color = terrainTextures[GRASS_COLOR].Sample(samplerStates[_ANISOTROPIC_SAMPLER_INDEX], tilling_coord);
    // ブレンド取得
    // g : ブレント率
    float blend_rate = saturate(heightTextures.Sample(samplerStates[_ANISOTROPIC_SAMPLER_INDEX], pin.texcoord).g);
    float4 color = lerp(rock_color, dirt_color, smoothstep(0.0f, 0.5f, blend_rate));
    color = lerp(color, grass_color, smoothstep(0.5f, 1.0f, blend_rate));
    return color;
}