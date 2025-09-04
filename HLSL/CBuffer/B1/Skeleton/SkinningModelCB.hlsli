#ifndef __SKINNING_MODEL_CONSTANT_BUFFER_H__
#define __SKINNING_MODEL_CONSTANT_BUFFER_H__

// スケルトン用定数バッファ
cbuffer CbSkeleton : register(b1)
{
    float4 materialColor;
    row_major float4x4 boneTransforms[256];
};

// スキニング処理
float4 SkinningPosition(float4 position, float4 boneWeights, uint4 boneIndices)
{
    float4 p = float4(0, 0, 0, 0);
    
    [unroll]
    for (int i = 0; i < 4; ++i)
    {
        p += (boneWeights[i] * mul(position, boneTransforms[boneIndices[i]]));
    }

    return p;
}

#endif  //  __SKINNING_MODEL_CONSTANT_BUFFER_H__