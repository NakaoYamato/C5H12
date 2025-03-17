// ピクセルシェーダーへの出力用構造体
struct PS_GB_OUT
{
    float4 baseColor      : SV_TARGET0;
    float4 worldNormal    : SV_TARGET1;
    float4 emissiveColor  : SV_TARGET2;
    float4 parameter      : SV_TARGET3;
    
    // depthはDepthStencilViewとして存在
    //float depth             : SV_TARGET5;
};

//------------------------------------------------------
// 書き込み用
PS_GB_OUT CreateOutputData(
float3 baseColor,
float specular,
float3 worldNormal,
float metallic,
float3 emissiveColor,
float roughness)
{
    PS_GB_OUT output = (PS_GB_OUT) 0;
    output.baseColor.rgb = baseColor;
    output.baseColor.a = specular;
    output.worldNormal.xyz = worldNormal;
    output.worldNormal.a = metallic;
    output.emissiveColor.rgb = emissiveColor;
    output.emissiveColor.a = roughness;
    return output;
}
// 書き込み用
//------------------------------------------------------

//------------------------------------------------------
// 読み込み用
// GBuffer用構造体
struct GBufferData
{
    float3 baseColor;
    float3 worldPosition;
    float3 worldNormal;
    float3 emissiveColor;
    float specular;
    float metallic;
    float roughness;
    float depth;
};

//  GBufferテクスチャ受け渡し用構造体
struct PSGBufferTextures
{
    Texture2D baseMap;
    Texture2D normalMap;
    Texture2D emissiveMap;
    Texture2D parameterMap;
    Texture2D depth;
    SamplerState state;
};

static const uint _BASE_COLOR_TEXTURE        = 0;
static const uint _WORLD_NORMAL_TEXTURE      = 1;
static const uint _EMISSIVE_COLOR_TEXTURE    = 2;
static const uint _PARAMETER_TEXTURE         = 3;
static const uint _DEPTH_TEXTURE             = 4;

static const uint _TEXTURE_MAX = _DEPTH_TEXTURE + 1;

//  ピクセルシェーダーの出力用構造体からGBufferData情報に変換
//  texturesはregister(t0)から開始している前提
GBufferData DecodeGBuffer(Texture2D textures[_TEXTURE_MAX], SamplerState state, float2 uv, matrix inverse_view_projection)
{
    //  各テクスチャから情報を取得
    float4 baseMapData      = textures[_BASE_COLOR_TEXTURE].Sample(state, uv);
    float4 normalMapData    = textures[_WORLD_NORMAL_TEXTURE].Sample(state, uv);
    float4 emissiveMapData  = textures[_EMISSIVE_COLOR_TEXTURE].Sample(state, uv);
    float4 parameterMapData = textures[_PARAMETER_TEXTURE].Sample(state, uv);
    float depth             = textures[_DEPTH_TEXTURE].Sample(state, uv).x;
    
    GBufferData ret;
    ret.baseColor = baseMapData.rgb;
    float4 position = float4(uv.x * 2.0f - 1.0f, uv.y * -2.0f + 1.0f, depth, 1);
    position = mul(position, inverse_view_projection);
    ret.worldPosition = position.xyz / position.w;
    ret.worldNormal = normalMapData.xyz;
    ret.emissiveColor = emissiveMapData.rgb;
    ret.specular = baseMapData.a;
    ret.metallic = normalMapData.a;
    ret.roughness = emissiveMapData.a;
    ret.depth = depth;
    return ret;
}
// 読み込み用
//------------------------------------------------------