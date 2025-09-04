#include "Grass.hlsli"
#include "../Instancing.hlsli"

VS_OUT main(VS_IN vin, uint instance_id : SV_INSTANCEID)
{
    vin.normal.w = 0;
    float sigma = vin.tangent.w;
    vin.tangent.w = 0;
    
    // ëêÇÃóhÇÍî{ó¶ÇéÊìæ
    float rate = 0.0f;
    switch (shakeAxis)
    {
        case 0: // +Xé≤
            rate = vin.position.x;
            break;
        case 1: // +Yé≤
            rate = vin.position.y;
            break;
        case 2: // +Zé≤
            rate = vin.position.z;
            break;
        case 3: // -Xé≤
            rate = -vin.position.x;
            break;
        case 4: // -Yé≤
            rate = -vin.position.y;
            break;
        case 5: // -Zé≤
            rate = -vin.position.z;
            break;
    }
    rate = rate * shakeAmplitude;
    
    // ïóÇÃÉVÅ[ÉhílåvéZ
    float seed = worldTransform[instance_id]._41 + worldTransform[instance_id]._42 + worldTransform[instance_id]._43;
    
    float4 position = vin.position;
    // ëêÇÃóhÇÍÇåvéZ
    position.xyz += windDirection * windStrength * sin(seed + totalElapsedTime * windSpeed) * rate;
    
    VS_OUT vout = (VS_OUT) 0;
    vout.world_position = mul(position, worldTransform[instance_id]);
    vout.world_normal = normalize(mul(vin.normal, worldTransform[instance_id]));
    vout.world_tangent = normalize(mul(vin.tangent, worldTransform[instance_id]));
    
    vout.position = mul(vout.world_position, viewProjection);
    vout.world_tangent.w = sigma;
    vout.binormal.xyz = normalize(cross(vout.world_normal.xyz, vout.world_tangent.xyz));
	
    vout.texcoord = vin.texcoord;
    
    vout.materialColor = materialColor[instance_id];
    
    return vout;
}