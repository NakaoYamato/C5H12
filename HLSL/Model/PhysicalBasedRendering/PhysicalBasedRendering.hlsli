#include "../ModelInputLayout.hlsli"

#include "../Light.hlsli"

// メッシュ定数バッファ
cbuffer MESH_CONSTANT_BUFFER : register(b2)
{
    float4 meshBaseColor;
    
    float roughnessFactor;
    float metalnessFactor;
    float2 dummy;
}

// 光源定数バッファ
cbuffer LIGHT_CONSTANT_BUFFER : register(b3)
{
    float4 world_ambient;
    float4 directional_light_direction;
    float4 directional_light_color;
    PointLight pointLights[8];
};

#include "../../Function/ShadingFunctions.hlsli"