#pragma once

#include "../../Library/Graphics/RenderContext.h"
#include "../../Library/Graphics/Shader.h"
#include "../../Library/Graphics/ConstantBuffer.h"
#include "../../Library/Material/Material.h"

/// <summary>
/// モデルの描画に使用するシェーダを指定するための基底クラス
/// </summary>
class ModelShaderBase
{
public:
    static constexpr int CBIndex = 2;

public:
	ModelShaderBase() = default;
    virtual ~ModelShaderBase() = default;

    // 開始処理
    virtual void Begin(const RenderContext& rc) = 0;

    // 更新処理
    virtual void Update(const RenderContext& rc, const Material* material) = 0;

    // 終了処理
    virtual void End(const RenderContext& rc)
    {
        ID3D11DeviceContext* dc = rc.deviceContext;

        // シェーダー設定解除
        dc->VSSetShader(nullptr, nullptr, 0);
        dc->PSSetShader(nullptr, nullptr, 0);
        dc->IASetInputLayout(nullptr);

        // 定数バッファ設定解除
        ID3D11Buffer* cbs[] = { nullptr };
        dc->PSSetConstantBuffers(CBIndex, _countof(cbs), cbs);

        // シェーダーリソースビュー設定解除
        ID3D11ShaderResourceView* srvs[] = { nullptr };
        dc->PSSetShaderResources(0, _countof(srvs), srvs);
    }

    // パラメータのkey取得
    virtual Material::ParameterMap GetParameterMap()const = 0;
};