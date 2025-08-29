#include "TerrainGrass.hlsli"
#include "../../Function/ToMatrix.hlsli"
#include "../../Function/Noise.hlsli"
#include "../../Define/SamplerStateDefine.hlsli"
SamplerState samplerStates[_SAMPLER_STATE_MAX] : register(s0);

#define PI 3.141592653
Texture2D distortion_texture : register(t5);

#define BLADE_SEGMENTS 5
[maxvertexcount(BLADE_SEGMENTS * 2 + 1)]
void main(point GRASS_GS_IN gin[1], inout TriangleStream<GRASS_PS_IN> output)
{
    if (gin[0].parameter.g <= 0.0f)
    {
        // マイナスなら描画しない
        return;
    }
    
    float cameraLength = length(cameraPosition.xyz - gin[0].worldPosition);
    if (cameraLength > grassLODDistanceMax)
    {
        // 遠すぎるなら描画しない
        return;
    }
        
#if 1
    const float randomXY = Random(gin[0].worldPosition.xy);
    const float randomYZ = Random(gin[0].worldPosition.yz);
    const float randomZX = Random(gin[0].worldPosition.zx);
    
    const float perlinNoise = Noise(gin[0].worldPosition.xyz * perlin_noise_distribution_factor);
    const float grassBladeHeight = gin[0].parameter.g + (perlinNoise * 2.0 - 1.0) * grass_height_variance;
    const float grassBladeWidth = grass_width_factor;
    const float4 witheredColor = float4(perlinNoise * grass_withered_factor, 0.0, 0.0, 1.0);
    
    float4 midpointPosition = float4(gin[0].worldPosition.xyz, 1.0f);
    midpointPosition.x += randomYZ;
    midpointPosition.z += randomXY;
    
    float4 midpointNormal = float4(normalize(gin[0].worldNormal), 0.0f);
    float4 midpointTangent = normalize(gin[0].worldTangent);
    
    const float2 distortion_texcoord = midpointPosition.xz + windFrequency * totalElapsedTime;
    const float4 distortion = distortion_texture.SampleLevel(samplerStates[_ANISOTROPIC_SAMPLER_INDEX], distortion_texcoord * 0.001, 0) * 2 - 1;
    const float wind_avatar_bending_angle = distortion.y * PI * 0.5 * windStrength;
    const float3 wind_bending_axis = normalize(float3(distortion.x, 0, distortion.z));
    const float4x4 W = ToMatrixRotation(wind_avatar_bending_angle, wind_bending_axis);

    const float4 avatar_offset = cameraPosition - midpointPosition;
    const float avatar_distance = length(avatar_offset);
    const float impact_radius = 5;
    const float avatar_bending_angle = smoothstep(impact_radius, 0, avatar_distance);
    const float3 avatar_bending_axis = normalize(cross(midpointNormal.xyz, normalize(avatar_offset).xyz));
    const row_major float4x4 A = ToMatrixRotation(avatar_bending_angle * PI * 0.2, avatar_bending_axis);
    const row_major float4x4 B = mul(W, A);
    
    const float random_curvature = Random(gin[0].worldPosition.xy * 0.01);
    float curvature = PI * 0.5 * (random_curvature * 2.0 - 1.0) * grass_curvature;
    const row_major float4x4 C = ToMatrixRotation(curvature / BLADE_SEGMENTS, midpointTangent.xyz);
    float4 segment_normal = midpointNormal;
    
    GRASS_PS_IN element;
    for (int i = 0; i < BLADE_SEGMENTS; i++)
    {
        float t = i / (float) BLADE_SEGMENTS;
        float segment_height = grassBladeHeight * t;
        float segment_width = grassBladeWidth * (1 - t);
        
        element.worldNormal = segment_normal.xyz;
        
        element.color = witheredColor;

        element.worldPosition = midpointPosition + segment_normal * segment_height;
        element.worldPosition.x += sin(randomZX * 2.0f * PI) * segment_width;
        element.worldPosition.z += cos(randomZX * 2.0f * PI) * segment_width;
        element.worldPosition = mul(element.worldPosition - midpointPosition, B) + midpointPosition;
        element.position = mul(element.worldPosition, viewProjection);
        element.texcoord = float2(0, 1 - t);
        output.Append(element);

        element.worldPosition = midpointPosition + segment_normal * segment_height;
        element.worldPosition = mul(element.worldPosition - midpointPosition, B) + midpointPosition;
        element.position = mul(element.worldPosition, viewProjection);
        element.texcoord = float2(1, 1 - t);
        output.Append(element);

        segment_normal = mul(segment_normal, C);
    }
    element.worldPosition = midpointPosition + segment_normal * grassBladeHeight;
    element.worldPosition = mul(element.worldPosition - midpointPosition, B) + midpointPosition;
    element.position = mul(element.worldPosition, viewProjection);
    element.worldNormal = segment_normal.xyz;
    element.color = witheredColor;
    element.texcoord = float2(0.5, 0);
    output.Append(element);

    output.RestartStrip();
#endif
    
#if 0
    float4x4 scaleMatrix = ToMatrixScaling(float3(0.1f, 0.1f, 0.1f));
    float4x4 rotationMatrix = ToMatrixRotationRollPitchYaw(float3(0.0f, 0.0f, 0.0f));
    float4x4 translationMatrix = ToMatrixTranslation(gin[0].worldPosition);
    float4x4 worldMatrix = mul(mul(scaleMatrix, rotationMatrix), translationMatrix);
    float4x4 worldVPMatrix = mul(worldMatrix, viewProjection);
    
    float4 color = 1.0f;
    
    //  頂点生成(ローカル座標)
    static const float4 vertexPositions[4] =
    {
        float4(-0.5f, -0.5f, 0, 1),
        float4(+0.5f, -0.5f, 0, 1),
        float4(-0.5f, +0.5f, 0, 1),
        float4(+0.5f, +0.5f, 0, 1),
    };
    static const float2 vertexTexcoord[4] =
    {
        float2(0, 1),
        float2(1, 1),
        float2(0, 0),
        float2(1, 0),
    };
    for (uint i = 0; i < 4; i++)
    {
        GRASS_PS_IN element;
        element.position = mul(vertexPositions[i], worldVPMatrix);
        element.worldPosition = mul(element.position, worldMatrix);
        element.worldNormal = gin[0].worldNormal;
        element.texcoord = vertexTexcoord[i];
        element.color = color;
        output.Append(element);
    }
    output.RestartStrip();
#endif
}