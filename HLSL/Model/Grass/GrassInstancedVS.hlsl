#include "../Phong/Phong.hlsli"

#include "../../CBuffer/B1/Skeleton/InstancingModelCB.hlsli"

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

VS_OUT main(VS_IN vin, uint instance_id : SV_INSTANCEID)
{
    InstancingModelData modelData = matrixBuffer[instance_id];
    vin.normal.w = 0;
    float sigma = vin.tangent.w;
    vin.tangent.w = 0;
    
    // 草の揺れ倍率を取得
    float rate = 0.0f;
    switch (shakeAxis)
    {
        case 0: // +X軸
            rate = vin.position.x;
            break;
        case 1: // +Y軸
            rate = vin.position.y;
            break;
        case 2: // +Z軸
            rate = vin.position.z;
            break;
        case 3: // -X軸
            rate = -vin.position.x;
            break;
        case 4: // -Y軸
            rate = -vin.position.y;
            break;
        case 5: // -Z軸
            rate = -vin.position.z;
            break;
    }
    rate = rate * shakeAmplitude;
    
    // 風のシード値計算
    float seed = modelData.worldTransform._41 + modelData.worldTransform._42 + modelData.worldTransform._43;
    
    float4 position = vin.position;
    // 草の揺れを計算
    position.xyz += windDirection * windStrength * sin(seed + totalElapsedTime * windSpeed) * rate;
    
    VS_OUT vout = (VS_OUT) 0;
    vout.world_position = mul(position, modelData.worldTransform);
    vout.world_normal = normalize(mul(vin.normal, modelData.worldTransform));
    vout.world_tangent = normalize(mul(vin.tangent, modelData.worldTransform));
    
    vout.position = mul(vout.world_position, viewProjection);
    vout.world_tangent.w = sigma;
    vout.binormal.xyz = normalize(cross(vout.world_normal.xyz, vout.world_tangent.xyz));
	
    vout.texcoord = vin.texcoord;
    
    vout.materialColor = modelData.materialColor;
    
    return vout;
}