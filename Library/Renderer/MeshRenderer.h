#pragma once

#include <unordered_map>
#include <vector>
#include <string>

#include "../Model/Model.h"
#include "../Shader/ShaderBase.h"

/// <summary>
/// モデルタイプ
/// </summary>
enum class ModelRenderType
{
    Dynamic,   // 骨の影響度がある(アニメーション等)
    Static,    // 骨の影響度がない(ステージ等のボーンがないモデル用)
    Instancing,    // GPUインスタンシング使用

    ModelRenderTypeMax
};

/// <summary>
/// ブレンドタイプ
/// </summary>
enum class BlendType
{
    Opaque,
    Alpha,

    BlendTypeMax
};

class MeshRenderer
{
private:
    static const int FBX_MAX_BONES = 256;
    static const int INSTANCED_MAX = 100;

#pragma region 定数バッファ
    struct DynamicBoneCB// 通常モデル用
    {
        DirectX::XMFLOAT4 materialColor{ 1,1,1,1 };
        DirectX::XMFLOAT4X4 boneTransforms[FBX_MAX_BONES]
    {
        {
            1,0,0,0,
            0,1,0,0,
            0,0,1,0,
            0,0,0,1
        }
    };
    };
    struct StaticBoneCB// アニメーションなしモデル用
    {
        DirectX::XMFLOAT4 materialColor{ 1,1,1,1 };
        DirectX::XMFLOAT4X4 world{};
    };
    struct InstancingModelCB// インスタンシングモデル用
    {
        DirectX::XMFLOAT4 materialColor[INSTANCED_MAX]{};
        DirectX::XMFLOAT4X4 world[INSTANCED_MAX]{};
    };
#pragma endregion

#pragma region Info
    struct DrawInfo
    {
        Model* model = nullptr;
        const ModelResource::Mesh* mesh = nullptr;
        Vector4				color{ 1,1,1,1 };
        ShaderBase::Parameter* parameter;
    };
    struct AlphaDrawInfo
    {
        Model* model = nullptr;
        const ModelResource::Mesh* mesh = nullptr;
        Vector4				color{ 1,1,1,1 };
        ShaderBase::Parameter* parameter;
        std::string shaderID;
        ModelRenderType renderType;
        float			distance = 0.0f;
    };
    // インスタンシング描画用
    struct InstancingDrawInfo
    {
        std::string				shaderId{};
        using ModelParameter = std::tuple<Vector4, DirectX::XMFLOAT4X4>;
        std::vector<ModelParameter> modelParameters;
        ShaderBase::Parameter* parameter = nullptr;
    };
#pragma endregion
public:
    /// <summary>
    /// 初期化
    /// </summary>
    /// <param name="device"></param>
    void Initialize(ID3D11Device* device);

    /// <summary>
    /// メッシュ描画
    /// </summary>
    /// <param name="mesh"></param>
    /// <param name="model"></param>
    /// <param name="color"></param>
    /// <param name="shaderId"></param>
    /// <param name="renderType"></param>
    void Draw(const ModelResource::Mesh* mesh,
        Model* model,
        const Vector4& color,
        std::string shaderId,
        ModelRenderType renderType,
        BlendType blendType,
        ShaderBase::Parameter* parameter);

    /// <summary>
    /// インスタンシングモデルの描画
    /// </summary>
    /// <param name="model"></param>
    /// <param name="color"></param>
    /// <param name="shaderId"></param>
    /// <param name="world"></param>
    void DrawInstancing(Model* model,
        const Vector4& color,
        std::string shaderId,
        const DirectX::XMFLOAT4X4& world,
        ShaderBase::Parameter* parameter);

    /// <summary>
    /// 描画実行
    /// シーン用定数バッファはこの関数を呼ぶ前に行う
    /// </summary>
    /// <param name="rc"></param>
    void RenderOpaque(const RenderContext& rc, bool writeGBuffer);

    /// <summary>
    /// 半透明描画
    /// </summary>
    /// <param name="rc"></param>
    void RenderAlpha(const RenderContext& rc);

    /// <summary>
    /// 影描画実行
    /// </summary>
    /// <param name="rc"></param>
    void CastShadow(const RenderContext& rc);

    /// <summary>
    /// ModelRenderTypeのシェーダー名を取得
    /// </summary>
    /// <param name="type"></param>
    /// <param name="deferred"></param>
    /// <returns></returns>
    std::vector<const char*> GetShaderNames(ModelRenderType type, bool deferred);

    /// <summary>
    /// typeとkeyからパラメータのkeyを取得
    /// </summary>
    /// <param name="type"></param>
    /// <param name="key"></param>
    /// <returns></returns>
    ShaderBase::Parameter GetShaderParameterKey(ModelRenderType type, std::string key, bool deferred);

private:
    // インスタンシングモデルの描画
    void RenderInstancing(const RenderContext& rc);

    // DynamicBoneModelのメッシュ描画
    void DrawDynamicBoneMesh(const RenderContext& rc,
        ShaderBase* shader,
        Model* model,
        const ModelResource::Mesh* mesh,
        const Vector4& materialColor,
        ShaderBase::Parameter* parameter);

    // StaticBoneModelのメッシュ描画
    void DrawStaticBoneModel(const RenderContext& rc,
        ShaderBase* shader,
        Model* model,
        const ModelResource::Mesh* mesh,
        const Vector4& materialColor,
        ShaderBase::Parameter* parameter);

private:
    // 定数バッファのデータ
    // サイズが大きいく関数内で定義するとスタック警告がでるため静的に確保
    DynamicBoneCB		_cbDynamicSkeleton{};
    StaticBoneCB		_cbStaticSkeleton{};
    InstancingModelCB	_cbInstancingSkeleton{};

    // シェーダーの配列
    using ShaderMap = std::unordered_map<std::string, std::unique_ptr<ShaderBase>>;
    ShaderMap                   _deferredShaders[static_cast<int>(ModelRenderType::ModelRenderTypeMax)];
    ShaderMap                   _forwardShaders[static_cast<int>(ModelRenderType::ModelRenderTypeMax)];
    std::unique_ptr<ShaderBase> _cascadedSMShader[static_cast<int>(ModelRenderType::ModelRenderTypeMax)];

    // 各モデルタイプのInfo
    using DrawInfoMap = std::unordered_map<std::string, std::vector<DrawInfo>>;
    using InstancingDrawInfoMap = std::unordered_map<Model*, InstancingDrawInfo>;
    DrawInfoMap                 _dynamicInfomap;
    DrawInfoMap                 _staticInfomap;
    std::vector<AlphaDrawInfo>	_alphaDrawInfomap;
    InstancingDrawInfoMap       _instancingInfoMap;

    // 各定数バッファ
    Microsoft::WRL::ComPtr<ID3D11Buffer>	_dynamicBoneCB;
    Microsoft::WRL::ComPtr<ID3D11Buffer>	_staticBoneCB;
    Microsoft::WRL::ComPtr<ID3D11Buffer>	_instancingCB;
};
