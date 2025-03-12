#include "PhysicalBasedRendering.hlsli"
#include "../../GBuffer/GBuffer.hlsli"
#define POINT 0
#define LINEAR 1
#define ANISOTROPIC 2
SamplerState samplerStates[3] : register(s0);

#define BASECOLOR_TEXTURE 0
#define ROUGHNESS_TEXTURE 1
#define NORMAL_TEXTURE 2
#define EMISSIVE_TEXTURE 3
Texture2D textureMaps[4] : register(t0);

//PS_GB_OUT main(VS_OUT pin)

float4 main() : SV_TARGET
{
	return float4(1.0f, 1.0f, 1.0f, 1.0f);
}