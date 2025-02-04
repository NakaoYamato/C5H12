// ピクセルシェーダーへの出力用構造体
struct PS_GB_OUT
{
    float4 diffuseColor     : SV_TARGET0;
    float4 ambientColor     : SV_TARGET1;
    float4 specularColor    : SV_TARGET2;
    float4 worldPosition    : SV_TARGET3;
    float4 worldNormal      : SV_TARGET4;
    
    // depthはDepthStencilViewとして存在
    //float depth             : SV_TARGET5;
};

#define DIFFUSE_COLOR_TEXTURE   0
#define AMBIENT_COLOR_TEXTURE   1
#define SPECULAR_COLOR_TEXTURE  2
#define WORLD_POSITION_TEXTURE  3
#define WORLD_NORMAL_TEXTURE    4
//#define DEPTH_TEXTURE           5

#define TEXTURE_MAX             5