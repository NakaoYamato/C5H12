#include "Phong.hlsli"
#include "../Instancing.hlsli"

VS_OUT main(VS_IN vin, uint instance_id : SV_INSTANCEID)
{
    vin.normal.w = 0;
    float sigma = vin.tangent.w;
    vin.tangent.w = 0;
    
    VS_OUT vout = (VS_OUT) 0;
    vout.world_position = mul(vin.position, worldTransform[instance_id]);
    vout.world_normal = normalize(mul(vin.normal, worldTransform[instance_id]));
    vout.world_tangent = normalize(mul(vin.tangent, worldTransform[instance_id]));
    
    vout.position = mul(vout.world_position, viewProjection);
    vout.world_tangent.w = sigma;
    vout.binormal.xyz = normalize(cross(vout.world_normal.xyz, vout.world_tangent.xyz));
	
    vout.texcoord = vin.texcoord;
    
    vout.materialColor = materialColor[instance_id];
    
    return vout;
}