// スケルトン用定数バッファ
static const int INSTANCING_MAX = 100;
cbuffer CbSkeleton : register(b1)
{
    float4 materialColor[INSTANCING_MAX];
    row_major float4x4 worldTransform[INSTANCING_MAX];
}
