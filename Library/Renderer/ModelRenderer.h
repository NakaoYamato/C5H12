#pragma once

#include <unordered_map>

#include "../3D/Model.h"
#include "../Shader/ShaderBase.h"

/// <summary>
/// 使用するシェーダタイプ
/// </summary>
enum class ShaderId
{
    Basic,
    Phong,
    CascadedShadowMap,

    ShaderIdMax
};

/// <summary>
/// 描画タイプ
/// </summary>
enum class ModelRenderType
{
    DynamicBoneModel,   // 骨の影響度がある(アニメーション等)
    StaticBoneModel,    // 骨の影響度がない(ステージ等のボーンがないモデル用)
    InstancingModel,    // GPUインスタンシング使用

    ModelRenderTypeMax
};

/// <summary>
/// モデルレンダラ
/// ゲープロの場合、描画するモデルごとにシェーダーを切り替えていたが
/// 各シェーダーごとに描画するモデルを保存することでシェーダー切り替えを最小限にしている
/// </summary>
class ModelRenderer
{
public:
    static const int FBX_MAX_BONES = 256;
    static const int INSTANCED_MAX = 100;

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

public:
    ModelRenderer(ID3D11Device* device);
    ~ModelRenderer() {}

    /// <summary>
    /// モデルの描画
    /// </summary>
    /// <param name="model"></param>
    /// <param name="color"></param>
    /// <param name="shaderId"></param>
    /// <param name="renderType"></param>
    void Draw(Model* model, const Vector4& color, ShaderId shaderId, ModelRenderType renderType);

    /// <summary>
    /// インスタンシングモデルの描画
    /// </summary>
    /// <param name="model"></param>
    /// <param name="color"></param>
    /// <param name="shaderId"></param>
    /// <param name="world"></param>
    void DrawInstancing(Model* model, const Vector4& color, ShaderId shaderId, const DirectX::XMFLOAT4X4& world);

    /// <summary>
    /// 描画実行
    /// </summary>
    /// <param name="rc"></param>
    void Render(const RenderContext& rc);

    /// <summary>
    /// 影描画実行
    /// </summary>
    /// <param name="rc"></param>
    void CastShadow(const RenderContext& rc);

private:
    // インスタンシングモデルの描画
    void RenderInstancing(const RenderContext& rc);

    // DynamicBoneModelのメッシュ描画
    void DrawDynamicBoneMesh(const RenderContext& rc,
        const DirectX::XMVECTOR& CameraPosition,
        const DirectX::XMVECTOR& CameraFront,
        Model* model, 
        const Vector4& materialColor, 
        ShaderBase* shader);

    // StaticBoneModelのメッシュ描画
    void DrawStaticBoneModel(const RenderContext& rc,
        const DirectX::XMVECTOR& CameraPosition,
        const DirectX::XMVECTOR& CameraFront,
        Model* model,
        const Vector4& materialColor,
        ShaderBase* shader);


private:
    struct DrawInfo
    {
        Model*                  model = nullptr;
        Vector4                 color{ 1,1,1,1 };
    };
    struct TransparencyDrawInfo
    {
        ShaderBase*             shader = nullptr;
        Model*                  model = nullptr;
        const ModelResource::Mesh* mesh = nullptr;
        Vector4                 color{ 1,1,1,1 };
        float					distance = 0.0f;
    };

    // インスタンシング描画用
    struct InstancingDrawInfo
    {
        ShaderId				shaderId{};
        using ModelParameter = std::pair<Vector4, DirectX::XMFLOAT4X4>;
        std::vector<ModelParameter> modelParameters;
    };

    // 定数バッファのデータ
    // サイズが大きいので関数内で定義するとスタック警告がでるためメンバ変数
    DynamicBoneCB cbSkeleton_{};
    StaticBoneCB cbBSkeleton_{};
    InstancingModelCB cbISkeleton_{};

    // シェーダーの配列
    using ShaderMap = std::unordered_map<ShaderId, std::unique_ptr<ShaderBase>>;
    ShaderMap shaders_[static_cast<int>(ModelRenderType::ModelRenderTypeMax)];

    // 各モデルタイプのInfo
    using DrawInfoMap = std::unordered_map<ShaderId, std::vector<DrawInfo>>;
    using InstancingDrawInfoMap = std::unordered_map<Model*, InstancingDrawInfo>;
    DrawInfoMap                             dynamicBoneDrawInfomap_;
    DrawInfoMap                             staticBoneDrawInfomap_;
    InstancingDrawInfoMap                   instancingDrawInfoMap_;

    // 透明描画
    std::vector<TransparencyDrawInfo>		dynamicTransparencies_;
    std::vector<TransparencyDrawInfo>		staticTransparencies_;

    // 各定数バッファ
    Microsoft::WRL::ComPtr<ID3D11Buffer>	dynamicBoneCB_;
    Microsoft::WRL::ComPtr<ID3D11Buffer>	staticBoneCB_;
    Microsoft::WRL::ComPtr<ID3D11Buffer>	instancingCB_;
};