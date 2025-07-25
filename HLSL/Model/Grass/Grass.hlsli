#include "../Phong/Phong.hlsli"

// 草の揺れ用定数バッファ
cbuffer GRASS_CONSTANT_BUFFER : register(b4)
{
    // 草が揺れる軸のタイプ
    // 0 : +X軸, 1 : +Y軸, 2 : +Z軸
    // 3 : -X軸, 4 : -Y軸, 5 : -Z軸
    int shakeAxis;
    float3 windDirection;
    
    float windSpeed;
    float shakeAmplitude;
    float2 grassPadding;
}