#include "SpriteDissolveShader.h"
#include "../../Graphics/GpuResourceManager.h"

SpriteDissolveShader::SpriteDissolveShader(ID3D11Device* device, D3D11_INPUT_ELEMENT_DESC* inputDescs, UINT inputSize)
{
	// 頂点シェーダー
	_vertexShader.Load(device, "./Data/Shader/HLSL/Sprite/SpriteVS.cso", inputDescs, inputSize);

	// ピクセルシェーダ
	_pixelShader.Load(device, "./Data/Shader/HLSL/Sprite/SpriteDissolvePS.cso");

	// 定数バッファ
	_cbDissolve.Create(device, sizeof(CBDissolve));

	// マスクテクスチャ
	_maskTexture.Load(device, L"./Data/Texture/Dissolve/Mask.png");
}

void SpriteDissolveShader::Begin(const RenderContext& rc)
{
	ID3D11DeviceContext* dc = rc.deviceContext;
	// シェーダー設定
	dc->IASetInputLayout(_vertexShader.GetInputLayout());
	dc->VSSetShader(_vertexShader.Get(), nullptr, 0);
	dc->PSSetShader(_pixelShader.Get(), nullptr, 0);
	// 定数バッファ設定
	ID3D11Buffer* cbs[] =
	{
		_cbDissolve.Get(),
	};
	dc->PSSetConstantBuffers(CBIndex, _countof(cbs), cbs);
	// テクスチャ設定
	ID3D11ShaderResourceView* srvs[] =
	{
		_maskTexture.Get(),
	};
	dc->PSSetShaderResources(10, _countof(srvs), srvs);
}

void SpriteDissolveShader::Update(const RenderContext& rc, const Material* material)
{
	// 定数バッファ更新
	CBDissolve dissolve{};
	if (material != nullptr)
	{
		if (auto param = material->GetParameterF1("amount"))
		{
			dissolve.amount = *param;
		}
		if (auto param = material->GetParameterF1("borderWidth"))
		{
			dissolve.borderWidth = *param;
		}
		if (auto param = material->GetParameterF2("maskScroll"))
		{
			dissolve.maskScroll = *param;
		}
		if (auto param = material->GetParameterF4("borderColor"))
		{
			dissolve.borderColor = *param;
		}
	}
	_cbDissolve.Update(rc.deviceContext, &dissolve);
}

void SpriteDissolveShader::End(const RenderContext& rc)
{
	SpriteShaderBase::End(rc);
	// テクスチャ解放
	ID3D11ShaderResourceView* nullSRV[] = { nullptr };
	rc.deviceContext->PSSetShaderResources(10, 1, nullSRV);
}

Material::ParameterMap SpriteDissolveShader::GetParameterMap() const
{
	Material::ParameterMap parameterMap;
	parameterMap["amount"] = CBDissolve().amount;
	parameterMap["borderWidth"] = CBDissolve().borderWidth;
	parameterMap["maskScroll"] = CBDissolve().maskScroll;
	parameterMap["borderColor"] = CBDissolve().borderColor;
	return parameterMap;
}
