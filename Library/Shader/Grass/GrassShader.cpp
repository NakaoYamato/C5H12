#include "GrassShader.h"
#include "../../HRTrace.h"
#include "../../Graphics/GpuResourceManager.h"

GrassShader::GrassShader(ID3D11Device* device, const char* vsName, const char* psName, D3D11_INPUT_ELEMENT_DESC* inputDescs, UINT inputSize)
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
	GpuResourceManager::CreatePsFromCso(device, psName,	_pixelShader.ReleaseAndGetAddressOf());

	// メッシュ用定数バッファ
	(void)GpuResourceManager::CreateConstantBuffer(device,
		sizeof(CbMesh),
		_meshConstantBuffer.ReleaseAndGetAddressOf());
	// グラス用定数バッファ
	(void)GpuResourceManager::CreateConstantBuffer(device,
		sizeof(CbGrass),
		_grassConstantBuffer.ReleaseAndGetAddressOf());
}

void GrassShader::Begin(const RenderContext& rc)
{
	ID3D11DeviceContext* dc = rc.deviceContext;

	// シェーダー設定
	dc->IASetInputLayout(_inputLayout.Get());
	dc->VSSetShader(_vertexShader.Get(),	nullptr, 0);
	dc->PSSetShader(_pixelShader.Get(),		nullptr, 0);

	// 定数バッファ設定
	ID3D11Buffer* cbs[] =
	{
		_meshConstantBuffer.Get(),
	};
	dc->PSSetConstantBuffers(CBIndex, _countof(cbs), cbs);
	dc->VSSetConstantBuffers(CBIndex + 2, 1, _grassConstantBuffer.GetAddressOf());
}

void GrassShader::Update(const RenderContext& rc, 
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
	// グラス用定数バッファ更新
	CbGrass cbGrass{};
	cbGrass.shakeAxis = static_cast<int>((*parameter)["shakeAxis"]);
	cbGrass.shakeAmplitude = (*parameter)["shakeAmplitude"];
	cbGrass.windSpeed = (*parameter)["windSpeed"];
	cbGrass.windDirection.x = (*parameter)["windDirection.x"];
	cbGrass.windDirection.y = (*parameter)["windDirection.y"];
	cbGrass.windDirection.z = (*parameter)["windDirection.z"];
	dc->UpdateSubresource(_grassConstantBuffer.Get(), 0, 0, &cbGrass, 0, 0);

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

void GrassShader::End(const RenderContext& rc)
{
	ID3D11DeviceContext* dc = rc.deviceContext;

	// シェーダー設定解除
	dc->VSSetShader(nullptr, nullptr, 0);
	dc->PSSetShader(nullptr, nullptr, 0);
	dc->IASetInputLayout(nullptr);

	// 定数バッファ設定解除
	ID3D11Buffer* cbs[] = { nullptr };
	dc->PSSetConstantBuffers(CBIndex, _countof(cbs), cbs);
	dc->VSSetConstantBuffers(CBIndex + 2, _countof(cbs), cbs);

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

ShaderBase::Parameter GrassShader::GetParameterKey() const
{
	CbGrass cb;
    ShaderBase::Parameter p;
    p["shakeAxis"] = static_cast<float>(cb.shakeAxis);
    p["shakeAmplitude"] = cb.shakeAmplitude;
    p["windSpeed"] = cb.windSpeed;
    p["windDirection.x"] = cb.windDirection.x;
    p["windDirection.y"] = cb.windDirection.y;
    p["windDirection.z"] = cb.windDirection.z;
    return p;
}
