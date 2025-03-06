#include "../ModelInputLayout.hlsli"

#include "../Light.hlsli"

class GRASS_VS_OUT
{
    float4 world_position : POSITION;
};

typedef GRASS_VS_OUT VS_CONTROL_POINT_OUTPUT;
typedef GRASS_VS_OUT HS_CONTROL_POINT_OUTPUT;
typedef GRASS_VS_OUT DS_OUT;

struct HS_CONSTANT_DATA_OUTPUT
{
    float tess_factor[3] : SV_TessFactor;
    float inside_tess_factor : SV_InsideTessFactor;
};
#define CONTROL_POINT_COUNT 3

struct GS_OUT
{
    float4 sv_position : SV_POSITION;
    float4 position : POSITION;
    float4 normal : NORMAL;
    float2 texcoord : TEXCOORD;
};

// メッシュ定数バッファ
cbuffer MESH_CONSTANT_BUFFER : register(b2)
{
    float tesselation_max_subdivision;
    float grass_blade_height;
    float grass_blade_width;
	// GRASS.02
    float noise_seed_multiplier;
    
    float lod_distance_max;
    float total_elapsed_time;
    float wind_directionX;
    float wind_directionZ;
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