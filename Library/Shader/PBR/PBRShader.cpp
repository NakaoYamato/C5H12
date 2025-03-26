#include "PBRShader.h"
#include "../../ResourceManager/GpuResourceManager.h"

PBRShader::PBRShader(ID3D11Device* device,
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

void PBRShader::Begin(const RenderContext& rc)
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

void PBRShader::Update(const RenderContext& rc, const ModelResource::Material* material, Parameter* parameter)
{
	ID3D11DeviceContext* dc = rc.deviceContext;

	Vector4 pbrFactor = material->colors.at("PBRFactor");
	// メッシュ用定数バッファ更新
	CbMesh cbMesh{};
	cbMesh.baseColor = material->colors.at("Diffuse");
	cbMesh.roughness = pbrFactor.y;
	cbMesh.metalness = pbrFactor.z;
	dc->UpdateSubresource(_meshConstantBuffer.Get(), 0, 0, &cbMesh, 0, 0);

	// シェーダーリソースビュー設定
	ID3D11ShaderResourceView* srvs[] =
	{
		material->textureDatas.at("Diffuse").textureSRV.Get(),
		material->textureDatas.at("Roughness").textureSRV.Get(),
		material->textureDatas.at("Normal").textureSRV.Get(),
		material->textureDatas.at("Emissive").textureSRV.Get()
	};
	dc->PSSetShaderResources(0, _countof(srvs), srvs);
}

void PBRShader::End(const RenderContext& rc)
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
	ID3D11ShaderResourceView* srvs[] =
	{
		nullptr,
		nullptr,
		nullptr,
		nullptr
	};
	dc->PSSetShaderResources(0, _countof(srvs), srvs);
}

ShaderBase::Parameter PBRShader::GetParameterKey() const
{
	ShaderBase::Parameter p;
	p["roughness"] = 0.1f;
	p["metalness"] = 0.0f;
    return p;
}
