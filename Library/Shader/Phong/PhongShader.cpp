#include "PhongShader.h"
#include "../../ResourceManager/GpuResourceManager.h"


PhongShader::PhongShader(ID3D11Device* device, 
	const char* vsName,
	const char* psName,
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
		psName,
		_pixelShader.ReleaseAndGetAddressOf());


	// メッシュ用定数バッファ
	(void)GpuResourceManager::CreateConstantBuffer(device,
		sizeof(CbMesh),
		_meshConstantBuffer.ReleaseAndGetAddressOf());
}

void PhongShader::Begin(const RenderContext& rc)
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
}

void PhongShader::Update(const RenderContext& rc, 
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
	ID3D11ShaderResourceView* srvs[] = 
	{
		material->GetTextureSRV("Diffuse"),
		material->GetTextureSRV("Normal"),
		material->GetTextureSRV("Specular"),
		material->GetTextureSRV("Roughness"),
		material->GetTextureSRV("Emissive")
	};
	dc->PSSetShaderResources(0, _countof(srvs), srvs);
}

void PhongShader::End(const RenderContext& rc)
{
	//ShaderBase::End(rc);
	ID3D11DeviceContext* dc = rc.deviceContext;

	// シェーダー設定解除
	dc->VSSetShader(nullptr, nullptr, 0);
	dc->PSSetShader(nullptr, nullptr, 0);
	dc->IASetInputLayout(nullptr);

	// 定数バッファ設定解除
	ID3D11Buffer* cbs[] = { nullptr };
	dc->PSSetConstantBuffers(CBIndex, _countof(cbs), cbs);

	// シェーダーリソースビュー設定解除
	ID3D11ShaderResourceView* srvs[] =
	{
		nullptr,
		nullptr,
		nullptr,
		nullptr,
		nullptr
	};
	dc->PSSetShaderResources(0, _countof(srvs), srvs);
}

ShaderBase::Parameter PhongShader::GetParameterKey() const
{
	ShaderBase::Parameter p;
	return p;
}
