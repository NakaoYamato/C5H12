#ifndef __DECAL_CONSTANT_BUFFER_H__
#define __DECAL_CONSTANT_BUFFER_H__

cbuffer GBUFFER_DECAL_CONSTANT_BUFFER : register(b2)
{
    row_major float4x4 world;
    row_major float4x4 invWorld;
    float4 materialColor;
    float4 decalDirection;
};

#endif  //  __DECAL_CONSTANT_BUFFER_H__