#include "Terrain.hlsli"
#include "../../HLSL/Function/Tessellation.hlsli"

// パッチ毎に適用される関数
HS_CONSTANT_OUT HSConstant(
InputPatch<HS_IN, 4> ip,
uint pid : SV_PrimitiveID)
{
    HS_CONSTANT_OUT hout = (HS_CONSTANT_OUT) 0;
    // カメラからの距離に応じて分割数を調整
    float4 center = (ip[0].position + ip[1].position + ip[2].position + ip[3].position) / 4.0;
    center = mul(center, world);
    float len = length(center.xyz - cameraPosition.xyz);
    int index = (int) clamp(len / lodDistance, 0.0f, 3.0f);
    float factor = lodTessFactors[index];
    
    // エッジの分割数を指定
    hout.factor[0] = factor;
    hout.factor[1] = factor;
    hout.factor[2] = factor;
    hout.factor[3] = factor;
    // 内部部分の分割数を指定
    hout.innerFactor[0] = factor;
    hout.innerFactor[1] = factor;
    return hout;
}

// コントロールポイントごとに適用される関数
[domain("quad")]
[partitioning("fractional_odd")] // integer fractional_odd fractional_even
[outputtopology("triangle_ccw")]
[outputcontrolpoints(4)]
[patchconstantfunc("HSConstant")]
DS_IN main(InputPatch<HS_IN, 4> input,
uint cpid : SV_OutputControlPointID)
{
    DS_IN hout = (DS_IN) 0;
    hout.position = input[cpid].position;
    hout.texcoord = input[cpid].texcoord;
    hout.normal = input[cpid].normal;
    return hout;
}