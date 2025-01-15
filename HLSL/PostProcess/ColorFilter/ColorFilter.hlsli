cbuffer COLOR_FILTER : register(b1)
{
    float hueShift; // F‘Š’²®
    float saturation; // Ê“x’²®
    float brightness; // –¾“x’²®
    float dummy;
};

#include "../../Sprite/Sprite.hlsli"
#include "FilterFunctions.hlsli"
