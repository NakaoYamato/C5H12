#include "RampShader.h"
#include "../../HRTrace.h"
#include "../../ResourceManager/GpuResourceManager.h"

RampShader::RampShader(ID3D11Device* device,
	const char* vsName, 
	D3D11_INPUT_ELEMENT_DESC* inputDescs,
	UINT inputSize)
{
	// 頂点シェーダー
	GpuResourceManager::CreateVsFromCso(
		device,
		vsName,
		_vertexShader.ReleaseAndGetAddressOf(),
		_inputLayout.ReleaseAndGetAddressOf(),
		inputDescs,
		inputSize);

	// ピクセルシェーダ
	GpuResourceManager::CreatePsFromCso(device,
		"./Data/Shader/RampPS.cso",
		_pixelShader.ReleaseAndGetAddressOf());


	// メッシュ用定数バッファ
	(void)GpuResourceManager::CreateConstantBuffer(device,
		sizeof(CbMesh),
		_meshConstantBuffer.ReleaseAndGetAddressOf());

	// ランプシェーディング用テクスチャ読み込み
	D3D11_TEXTURE2D_DESC texture2dDesc{};
	GpuResourceManager::LoadTextureFromFile(device,
		L"./Data/Texture/Ramp/ramp.png",
		_rampSRV.ReleaseAndGetAddressOf(),
		&texture2dDesc);
}

void RampShader::Begin(const RenderContext& rc)
{
	ID3D11DeviceContext* dc = rc.deviceContext;

	// シェーダー設定
	dc->IASetInputLayout(_inputLayout.Get());
	dc->VSSetShader(_vertexShader.Get(), nullptr, 0);
	dc->PSSetShader(_pixelShader.Get(), nullptr, 0);

	// 定数バッファ設定
	ID3D11Buffer* cbs[] =
	{
		_meshConstantBuffer.Get(),
	};
	dc->PSSetConstantBuffers(CBIndex, _countof(cbs), cbs);

	// ランプシェーディング用テクスチャ設定
	dc->PSSetShaderResources(5, 1, _rampSRV.GetAddressOf());
}

void RampShader::Update(const RenderContext& rc,
	const Material* material,
	Parameter* parameter)
{
	ID3D11DeviceContext* dc = rc.deviceContext;

	// メッシュ用定数バッファ更新
	CbMesh cbMesh{};
	cbMesh.Ka = material->GetColor("Ambient");
	cbMesh.Kd = material->GetColor("Diffuse");
	cbMesh.Ks = material->GetColor("Specular");
	dc->UpdateSubresource(_meshConstantBuffer.Get(), 0, 0, &cbMesh, 0, 0);

	// シェーダーリソースビュー設定
	dc->PSSetShaderResources(0, 1, material->GetAddressOfTextureSRV("Diffuse"));
	dc->PSSetShaderResources(1, 1, material->GetAddressOfTextureSRV("Normal"));
}

void RampShader::End(const RenderContext& rc)
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
	ID3D11ShaderResourceView* srvs[] = { nullptr, nullptr, nullptr };
	dc->PSSetShaderResources(0, _countof(srvs), srvs);

	ID3D11ShaderResourceView* rampsrvs[] = { nullptr };
	dc->PSSetShaderResources(5, 1, rampsrvs);
}

ShaderBase::Parameter RampShader::GetParameterKey() const
{
	ShaderBase::Parameter p;
	p["test2"] = 0.0f;
	return p;
}
