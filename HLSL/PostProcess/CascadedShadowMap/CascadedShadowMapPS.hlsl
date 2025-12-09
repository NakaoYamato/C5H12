#include "../../Sprite/Sprite.hlsli"
#include "../../CBuffer/B0/SceneCB.hlsli"

SamplerState borderPointState : register(s4);
SamplerComparisonState comparisonSamplerState : register(s5);

Texture2D colorMap : register(t0);
Texture2D depthMap : register(t1);
Texture2DArray cascadedShadowMaps : register(t2);

cbuffer PARAMETRIC_CONSTANT_BUFFER : register(b2)
{
	// CASCADED_SHADOW_MAPS
    float shadowColor;
    float shadowDepthBias;
    float colorizeCascadedLayer;
    float parametricPadding;
};
cbuffer CSM_CONSTANT_BUFFER : register(b3)
{
    row_major float4x4 cascadedMatrices[4];
    float4 cascadedPlaneDistances;
}

float4 main(VsOut pin) : SV_TARGET
{
    float4 sampledColor = colorMap.Sample(borderPointState, pin.texcoord);
    float3 color = sampledColor.rgb;
    float alpha = sampledColor.a;
	
    float depthNDC = depthMap.Sample(borderPointState, pin.texcoord).x;

    float4 positionNDC;
    // テクスチャ座標 (0 ~ 1) から NDC (-1 ~ 1) へ変換
    positionNDC.x = pin.texcoord.x * +2 - 1;
    positionNDC.y = pin.texcoord.y * -2 + 1;
    positionNDC.z = depthNDC;
    positionNDC.w = 1;

    // NDCからビュー空間へ変換
    float4 viewPosition = mul(positionNDC, invProjection);
    viewPosition = viewPosition / viewPosition.w;
	
    // NDCからワールド空間へ変換
    float4 worldPosition = mul(positionNDC, invViewProjection);
    worldPosition = worldPosition / worldPosition.w;
    
    // カスケードシャドウマップの適用
    float viewDepth = viewPosition.z;
    int cascadeIndex = -1;
    for (uint layer = 0; layer < 4; ++layer)
    {
        float distance = cascadedPlaneDistances[layer];
        if (distance > viewDepth)
        {
            cascadeIndex = layer;
            break;
        }
    }
    float shadow_factor = 1.0;
    if (cascadeIndex > -1)
    {
        // ワールド座標をライトの射影空間(NDC)へ変換
        float4 lightSpacePosition = mul(worldPosition, cascadedMatrices[cascadeIndex]);
        lightSpacePosition /= lightSpacePosition.w;

        // NDC (-1 ~ 1) を テクスチャ座標 (0 ~ 1) に変換
        lightSpacePosition.x = lightSpacePosition.x * +0.5 + 0.5;
        lightSpacePosition.y = lightSpacePosition.y * -0.5 + 0.5;

        // シャドウマップのテクセルサイズを取得
        float w, h, elements;
        cascadedShadowMaps.GetDimensions(w, h, elements);
        float2 texelSize = 1.0f / float2(w, h);

        // PCFフィルタリング (3x3 カーネル)
        float shadowSum = 0.0;
        float currentDepth = lightSpacePosition.z - shadowDepthBias;

        // -1, 0, +1 の範囲でループ（計9回サンプリング）
        [unroll]
        for (int x = -1; x <= 1; ++x)
        {
            [unroll]
            for (int y = -1; y <= 1; ++y)
            {
                float2 offset = float2(x, y) * texelSize;
                
                // 配列テクスチャ(Texture2DArray)なので、Z成分に cascade_index を指定
                shadowSum += cascadedShadowMaps.SampleCmpLevelZero(
                    comparisonSamplerState,
                    float3(lightSpacePosition.xy + offset, cascadeIndex),
                    currentDepth
                ).r;
            }
        }

        // 9回のサンプリング結果を平均化
        shadow_factor = shadowSum / 9.0;

        // 影の色を適用
        color *= lerp(shadowColor, 1.0, shadow_factor);
        
#if 1   // シャドウマップの各エリアを可視化
        if (colorizeCascadedLayer)
        {
            const float3 layer_colors[4] =
            {
                { 1, 0, 0 },
                { 0, 1, 0 },
                { 0, 0, 1 },
                { 1, 1, 0 },
            };
            color *= layer_colors[cascadeIndex];
        }
#endif        
    }
	
    return float4(color, alpha);
}