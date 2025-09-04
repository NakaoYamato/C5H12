#ifndef __LIGHT_CONSTANT_BUFFER_H__
#define __LIGHT_CONSTANT_BUFFER_H__

#include "../../Define/Light.hlsli"

// 光源定数バッファ
cbuffer LIGHT_CONSTANT_BUFFER : register(b3)
{
    float4 world_ambient;
    float4 directional_light_direction;
    float4 directional_light_color;
    PointLight pointLights[8];
};

#endif  //  __LIGHT_CONSTANT_BUFFER_H__