#include "../ModelInputLayout.hlsli"
#include "../Skinning.hlsli"

VS_OUT main(VS_IN vin)
{
    VS_OUT vout = (VS_OUT)0;
    vout.world_position = SkinningPosition(vin.position, vin.boneWeights, vin.boneIndices);
    
    vout.position = mul(vout.world_position, view_projection);
	
    vout.texcoord = vin.texcoord;
    vout.materialColor = materialColor;
	
    return vout;
}