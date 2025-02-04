// ピクセルシェーダーへの出力用構造体
struct PS_GB_OUT
{
    float4 diffuseColor     : SV_TARGET0;
    //float4 ambientColor     : SV_TARGET1;
    float4 colorFactor    : SV_TARGET1;
    float4 worldPosition    : SV_TARGET2;
    float4 worldNormal      : SV_TARGET3;
    
    // depthはDepthStencilViewとして存在
    //float depth             : SV_TARGET5;
};

#define DIFFUSE_COLOR_TEXTURE   0
//#define AMBIENT_COLOR_TEXTURE   1
#define COLOR_FACTOR_TEXTURE  1
#define WORLD_POSITION_TEXTURE  2
#define WORLD_NORMAL_TEXTURE    3
#define DEPTH_TEXTURE           4

#define TEXTURE_MAX             5

PS_GB_OUT CreateOutputData(float4 diffuseColor, float specular, float4 worldPosition, float4 worldNormal)
{
    PS_GB_OUT output = (PS_GB_OUT) 0;
    output.diffuseColor = diffuseColor;
    output.colorFactor.x = specular;
    output.worldPosition = worldPosition;
    output.worldNormal = worldNormal;
    return output;
}