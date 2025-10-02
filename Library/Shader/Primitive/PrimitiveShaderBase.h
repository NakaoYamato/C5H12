#pragma once

#include "../../Library/Graphics/RenderContext.h"
#include "../../Library/Graphics/Shader.h"
#include "../../Library/Graphics/ConstantBuffer.h"
#include "../../Library/Material/Material.h"

class PrimitiveShaderBase
{
public:
	PrimitiveShaderBase() = default;
	virtual ~PrimitiveShaderBase() = default;
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
		// シェーダーリソースビュー設定解除
		ID3D11ShaderResourceView* srvs[] = { nullptr };
		dc->PSSetShaderResources(0, _countof(srvs), srvs);
	}

	// パラメータのkey取得
	virtual Material::ParameterMap GetParameterMap()const = 0;

protected:
	VertexShader 	_vertexShader;
	PixelShader		_pixelShader;
};