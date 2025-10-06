#include "Phong.hlsli"
#include "../../CBuffer/B1/Skeleton/InstancingModelCB.hlsli"

VS_OUT main(VS_IN vin, uint instance_id : SV_INSTANCEID)
{
    InstancingModelData modelData = matrixBuffer[instance_id];
    
    vin.normal.w = 0;
    float sigma = vin.tangent.w;
    vin.tangent.w = 0;
    
    VS_OUT vout = (VS_OUT) 0;
    vout.world_position = mul(vin.position, modelData.worldTransform);
    vout.world_normal = normalize(mul(vin.normal, modelData.worldTransform));
    vout.world_tangent = normalize(mul(vin.tangent, modelData.worldTransform));
    
    vout.position = mul(vout.world_position, viewProjection);
    vout.world_tangent.w = sigma;
    vout.binormal.xyz = normalize(cross(vout.world_normal.xyz, vout.world_tangent.xyz));
	
    vout.texcoord = vin.texcoord;
    
    vout.materialColor = modelData.materialColor;
    
    return vout;
}