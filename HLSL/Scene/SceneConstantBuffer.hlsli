#ifndef __SCENE_CONSTANT_BUFFER_H__
#define __SCENE_CONSTANT_BUFFER_H__

// シーン定数バッファ
cbuffer SCENE_CONSTANT_BUFFER : register(b0)
{
    row_major float4x4 view;
    row_major float4x4 projection;
    row_major float4x4 viewProjection;
    row_major float4x4 invView;
    row_major float4x4 invProjection;
    row_major float4x4 invViewProjection;
    float4 lightDirection;
    float4 lightColor;
    float4 cameraPosition;
    
    float totalElapsedTime;
    float deltaTime;
    float windFrequency;
    float windStrength;
}

#endif  //  __SCENE_CONSTANT_BUFFER_H__