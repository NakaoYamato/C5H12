#include "SpriteScrollShader.h"
#include "../../Graphics/GpuResourceManager.h"

SpriteScrollShader::SpriteScrollShader(ID3D11Device* device,
	D3D11_INPUT_ELEMENT_DESC* inputDescs, UINT inputSize)
{
	// 頂点シェーダー
	_vertexShader.Load(device, "./Data/Shader/HLSL/Sprite/SpriteVS.cso", inputDescs, inputSize);

	// ピクセルシェーダ
	_pixelShader.Load(device, "./Data/Shader/HLSL/Sprite/SpriteScrollPS.cso");

	// 定数バッファ
	_cbScroll.Create(device, sizeof(CBScroll));
}

// 開始処理
void SpriteScrollShader::Begin(const RenderContext& rc)
{
	ID3D11DeviceContext* dc = rc.deviceContext;
	// シェーダー設定
	dc->IASetInputLayout(_vertexShader.GetInputLayout());
	dc->VSSetShader(_vertexShader.Get(), nullptr, 0);
	dc->PSSetShader(_pixelShader.Get(), nullptr, 0);

	// 定数バッファ設定
	ID3D11Buffer* cbs[] =
	{
		_cbScroll.Get(),
	};
	dc->PSSetConstantBuffers(CBIndex, _countof(cbs), cbs);
}

// 更新処理
void SpriteScrollShader::Update(const RenderContext& rc, const Material* material)
{
	// 定数バッファ更新
	CBScroll scroll{};
	if (material != nullptr)
	{
		if (auto param = material->GetParameterF1("scrollU"))
		{
			scroll.scrollU = *param;
		}
		if (auto param = material->GetParameterF1("scrollV"))
		{
			scroll.scrollV = *param;
		}
	}
	_cbScroll.Update(rc.deviceContext, &scroll);
}

// 終了処理
void SpriteScrollShader::End(const RenderContext& rc)
{
	SpriteShaderBase::End(rc);
}

// パラメータのkey取得
Material::ParameterMap SpriteScrollShader::GetParameterMap() const
{
	Material::ParameterMap p;
	p["scrollU"] = 0.0f;
	p["scrollV"] = 0.0f;
	return p;
}
