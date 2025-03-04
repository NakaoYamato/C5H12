#pragma once

#include <unordered_map>

#include "../3D/Model.h"
#include "../Shader/ShaderBase.h"

/// <summary>
/// 使用するシェーダタイプ
/// </summary>
enum class ShaderId
{
    Phong,
    Ramp,
    CascadedShadowMap,

    ShaderIdMax
};

/// <summary>
/// 描画タイプ
/// </summary>
enum class ModelRenderType
{
    Dynamic,   // 骨の影響度がある(アニメーション等)
    Static,    // 骨の影響度がない(ステージ等のボーンがないモデル用)
    Instancing,    // GPUインスタンシング使用

    ModelRenderTypeMax
};

namespace ModelRenderer
{
    void Initialize(ID3D11Device* device);

    /// <summary>
    /// モデルの描画
    /// </summary>
    /// <param name="model"></param>
    /// <param name="color"></param>
    /// <param name="shaderId"></param>
    /// <param name="renderType"></param>
    void Draw(Model* model, const Vector4& color, ShaderId shaderId, ModelRenderType renderType);

    /// <summary>
    /// メッシュ描画
    /// </summary>
    /// <param name="mesh"></param>
    /// <param name="model"></param>
    /// <param name="color"></param>
    /// <param name="shaderId"></param>
    /// <param name="renderType"></param>
    void DrawMesh(const ModelResource::Mesh* mesh,
        Model* model,
        const Vector4& color,
        ShaderId shaderId, ModelRenderType renderType);

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
}
