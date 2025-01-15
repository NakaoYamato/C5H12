#include "Phong.hlsli"
#include "../Skinning.hlsli"

VS_OUT main(VS_IN vin)
{
    vin.normal.w = 0;
    float sigma = vin.tangent.w;
    vin.tangent.w = 0;
    
    VS_OUT vout = (VS_OUT) 0;
    
    // アニメーション処理
    vout.world_position = SkinningPosition(vin.position, vin.boneWeights, vin.boneIndices);
    vout.world_normal = normalize(SkinningPosition(vin.normal, vin.boneWeights, vin.boneIndices));
    vout.world_tangent = normalize(SkinningPosition(vin.tangent, vin.boneWeights, vin.boneIndices));
    
    vout.position = mul(vout.world_position, view_projection);
    vout.world_tangent.w = sigma;
    vout.binormal.xyz = normalize(cross(vout.world_normal.xyz, vout.world_tangent.xyz));
	
    vout.texcoord = vin.texcoord;
    vout.materialColor = materialColor;
	
    return vout;
}