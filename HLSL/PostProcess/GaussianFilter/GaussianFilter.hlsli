#define KERNEL_MAX 25

#include "../../Sprite/Sprite.hlsli"

cbuffer GAUSSIAN_FILTER : register(b1)
{
    float4 weights[KERNEL_MAX * KERNEL_MAX];
    float kernelSize;
    float2 texcel;
    float dummy;
}