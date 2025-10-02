#include "LocusPrimitiveShader.h"

LocusPrimitiveShader::LocusPrimitiveShader(ID3D11Device* device, D3D11_INPUT_ELEMENT_DESC* inputDescs, UINT inputSize)
{
	// 頂点シェーダー
	_vertexShader.Load(device, "./Data/Shader/HLSL/PrimitiveRenderer/PrimitiveRendererVS.cso", inputDescs, inputSize);
	// ピクセルシェーダ読み込み
	_pixelShader.Load(device, "./Data/Shader/HLSL/PrimitiveRenderer/Locus/PlayerSwordLocusPS.cso");
}

// 開始処理
void LocusPrimitiveShader::Begin(const RenderContext& rc)
{
	ID3D11DeviceContext* dc = rc.deviceContext;
	// シェーダー設定
	dc->VSSetShader(_vertexShader.Get(), nullptr, 0);
	dc->IASetInputLayout(_vertexShader.GetInputLayout());
	dc->PSSetShader(_pixelShader.Get(), nullptr, 0);
}

// 更新処理
void LocusPrimitiveShader::Update(const RenderContext& rc, const Material* material)
{
	ID3D11DeviceContext* dc = rc.deviceContext;
	// テクスチャ設定
	ID3D11ShaderResourceView* srvs[] =
	{
		material->GetTextureData("Diffuse").Get(),
		material->GetTextureData("Noise").Get(),
	};
	dc->PSSetShaderResources(0, _countof(srvs), srvs);
}

// 終了処理
void LocusPrimitiveShader::End(const RenderContext& rc)
{
	ID3D11DeviceContext* dc = rc.deviceContext;
	// シェーダ解除
	dc->VSSetShader(NULL, 0, 0);
	dc->PSSetShader(NULL, 0, 0);
	// テクスチャ設定
	ID3D11ShaderResourceView* srvs[] =
	{
		nullptr,
		nullptr,
	};
	dc->PSSetShaderResources(0, _countof(srvs), srvs);
}

// パラメータのkey取得
Material::ParameterMap LocusPrimitiveShader::GetParameterMap() const
{
	Material::ParameterMap p;
	return Material::ParameterMap();
}
