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
		vertexShader_.ReleaseAndGetAddressOf(),
		inputLayout_.ReleaseAndGetAddressOf(),
		inputDescs,
		inputSize);

	// ピクセルシェーダ
	GpuResourceManager::CreatePsFromCso(device,
		psName,
		pixelShader_.ReleaseAndGetAddressOf());


	// メッシュ用定数バッファ
	(void)GpuResourceManager::CreateConstantBuffer(device,
		sizeof(CbMesh),
		meshConstantBuffer_.ReleaseAndGetAddressOf());
}

void PhongShader::Begin(const RenderContext& rc)
{
	ID3D11DeviceContext* dc = rc.deviceContext;

	// シェーダー設定
	dc->IASetInputLayout(inputLayout_.Get());
	dc->VSSetShader(vertexShader_.Get(), nullptr, 0);
	dc->PSSetShader(pixelShader_.Get(), nullptr, 0);

	// 定数バッファ設定
	ID3D11Buffer* cbs[] =
	{
		meshConstantBuffer_.Get(),
	};
	dc->PSSetConstantBuffers(2, _countof(cbs), cbs);
}

void PhongShader::Update(const RenderContext& rc, const ModelResource::Material* material)
{
	ID3D11DeviceContext* dc = rc.deviceContext;

	// メッシュ用定数バッファ更新
	CbMesh cbMesh{};
	cbMesh.Ka = material->colors.at("Ambient");
	cbMesh.Kd = material->colors.at("Diffuse");
	cbMesh.Ks = material->colors.at("Specular");
	dc->UpdateSubresource(meshConstantBuffer_.Get(), 0, 0, &cbMesh, 0, 0);

	// シェーダーリソースビュー設定
	dc->PSSetShaderResources(0, 1, material->textureDatas.at("Diffuse").textureSRV.GetAddressOf());
	dc->PSSetShaderResources(1, 1, material->textureDatas.at("Normal").textureSRV.GetAddressOf());
	dc->PSSetShaderResources(2, 1, material->textureDatas.at("Specular").textureSRV.GetAddressOf());
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
	dc->PSSetConstantBuffers(1, _countof(cbs), cbs);

	// シェーダーリソースビュー設定解除
	ID3D11ShaderResourceView* srvs[] = { nullptr, nullptr, nullptr };
	dc->PSSetShaderResources(0, _countof(srvs), srvs);
}
