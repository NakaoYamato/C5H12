#include "TerrainGrass.hlsli"
#include "../../Function/ToMatrix.hlsli"
#include "../../Function/Noise.hlsli"
#include "../../Define/SamplerStateDefine.hlsli"
#include "../../CBuffer/B5/Object.hlsli"
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
    
    // TODO : カメラからの距離が一定以上なら動かない草を描画
    
    //float cameraLength = length(cameraPosition.xyz - gin[0].worldPosition);
    //if (cameraLength > grassLODDistanceMax)
    //{
    //    // 遠すぎるなら描画しない
    //    return;
    //}
        
#if 1
    // ランダム値の生成
    const float randomXY = Random(gin[0].worldPosition.xy);
    const float randomYZ = Random(gin[0].worldPosition.yz);
    const float randomZX = Random(gin[0].worldPosition.zx);
    
    // ノイズから草の高さ、幅、曲率、先細り率を決定
    const float perlinNoise = Noise(gin[0].worldPosition.xyz * perlinNoiseDistributionFactor);
    const float noise01 = smoothstep(0.0f, 1.0f, perlinNoise * 2.0 - 1.0);
    const float grassBladeHeight = gin[0].parameter.g + grassHeightFactor + noise01 * grassHeightVariance;
    const float grassBladeWidth = grassWidthFactor + noise01 * grassWidthVariance;
    float4 witheredColor = float4(perlinNoise * grassWitheredFactor, 0.0, 0.0, 1.0);
    const float curvature = grassCurvature + noise01 * grassCurvatureVariance;
    const float taper = grassTaperFactor + noise01 * grassTaperVariance;
    
    // 草の中点の位置を計算
    float4 midpointPosition = float4(gin[0].worldPosition.xyz, 1.0f);
    midpointPosition.x += randomYZ;
    midpointPosition.z += randomXY;
    
    float4 midpointNormal = float4(normalize(gin[0].worldNormal), 0.0f);
    float4 midpointTangent = normalize(gin[0].worldTangent);
    
    float3 rightVector = float3(sin(randomZX * 2.0f * PI), 0.0f, cos(randomZX * 2.0f * PI));
    float3 forwardVector = normalize(cross(float3(0, 1, 0), rightVector));
    
    // 風による歪みをテクスチャから取得
    const float2 distortionTexcoord = midpointPosition.xz + windFrequency * totalElapsedTime;
    const float4 distortion = distortion_texture.SampleLevel(samplerStates[_ANISOTROPIC_SAMPLER_INDEX], distortionTexcoord * grassWindVariance, 0) * 2 - 1;
    const float windAngle = distortion.y * PI * 0.5 * windStrength;
    const float3 windAxis = normalize(float3(distortion.x, 0, distortion.z));
    const float4x4 W = ToMatrixRotation(windAngle, windAxis);

    // カメラからの距離に応じて草が倒れる角度を決定
    const float4 avatarOffset = cameraPosition - midpointPosition;
    const float avatarDistance = length(avatarOffset);
    const float impactRadius = 5;
    const float avatarBendingAngle = smoothstep(impactRadius, 0, avatarDistance);
    const float3 avatarBendingAxis = normalize(cross(midpointNormal.xyz, normalize(avatarOffset).xyz));
    const row_major float4x4 A = ToMatrixRotation(avatarBendingAngle * PI * 0.2, avatarBendingAxis);
    const row_major float4x4 B = W;
    
    // ランダムな曲がり具合を加える
    const float randomCurvature = Random(gin[0].worldPosition.xy * 0.01);
    float curvatureAngle = PI * 0.5 * (randomCurvature * 2.0 - 1.0) * curvature;
    const row_major float4x4 C = ToMatrixRotation(curvatureAngle / BLADE_SEGMENTS, forwardVector);
    float4 segmentNormal = float4(0, 1, 0, 0);
    
    // 障害物との位置関係から倒れる方向を決定
    float3 fallVector = float3(0, 0, 0);
    
    [unroll]
    for (int objectIndex = 0; objectIndex < MaxObjectConstant; objectIndex++)
    {
        if (objectData[objectIndex].radius > 0.0f)
        {
            float3 toObstacle = objectData[objectIndex].position - midpointPosition.xyz;
            float len = length(toObstacle);
            float lenParRadius = len / objectData[objectIndex].radius;
            if (lenParRadius < 1.0f)
            {
                fallVector -= (1.0f - lenParRadius) * normalize(toObstacle);
            }
        }
    }
    // 倒れる方向のy成分を減らす
    fallVector.y = 0.0f;
    
    GRASS_PS_IN element;
    for (int i = 0; i < BLADE_SEGMENTS; i++)
    {
        float t = i / (float) BLADE_SEGMENTS;
        float segment_height = grassBladeHeight * t;
        float segment_width = grassBladeWidth * (1 - t * taper);
        
        element.worldNormal = segmentNormal.xyz;
        
        element.color = witheredColor;
        float4 centerPosition = midpointPosition + segmentNormal * segment_height;
        // 倒れる方向を加える
        centerPosition.xyz += fallVector.xyz * i * obstacleInfluence;
        
        // 1つ目の頂点（左側）
        element.worldPosition = centerPosition;
        element.worldPosition.xyz += rightVector * segment_width;
        
        element.worldPosition = mul(element.worldPosition - midpointPosition, B) + midpointPosition;
        element.position = mul(element.worldPosition, viewProjection);
        element.texcoord = float2(0, 1 - t);
        output.Append(element);
        
        // 2つ目の頂点（右側）
        element.worldPosition = centerPosition;
        element.worldPosition.xyz -= rightVector * segment_width;
        
        element.worldPosition = mul(element.worldPosition - midpointPosition, B) + midpointPosition;
        element.position = mul(element.worldPosition, viewProjection);
        element.texcoord = float2(1, 1 - t);
        output.Append(element);

        segmentNormal = mul(segmentNormal, C);
    }
    element.worldPosition = midpointPosition + segmentNormal * grassBladeHeight;
    // 倒れる方向を加える
    element.worldPosition.xyz += fallVector.xyz * BLADE_SEGMENTS * obstacleInfluence;
    element.worldPosition = mul(element.worldPosition - midpointPosition, B) + midpointPosition;
    element.position = mul(element.worldPosition, viewProjection);
    // 草が伸びる方向(Up)と横方向(Right)の外積から、草の「面」が向いている法線(Forward)を計算
    float3 faceNormal = normalize(cross(segmentNormal.xyz, rightVector));

    // その面法線を頂点にセットする
    element.worldNormal = faceNormal;
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
        element.color.g = gin[0].texcoord.x;
        element.color.r = gin[0].texcoord.y;
        element.color.b = 0.0f;
        output.Append(element);
    }
    output.RestartStrip();
#endif
}