#pragma once

#include <unordered_map>
#include <vector>
#include <string>

#include "../3D/Model.h"
#include "../Shader/ShaderBase.h"

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

namespace MeshRenderer
{
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
}
