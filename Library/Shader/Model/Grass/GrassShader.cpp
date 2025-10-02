#include "GrassShader.h"
#include "../../HRTrace.h"
#include "../../Graphics/GpuResourceManager.h"

GrassShader::GrassShader(ID3D11Device* device, const char* vsName, const char* psName, D3D11_INPUT_ELEMENT_DESC* inputDescs, UINT inputSize)
{
	// 頂点シェーダー
	_vertexShader.Load(device, vsName, inputDescs, inputSize);

	// ピクセルシェーダ
	_pixelShader.Load(device, psName);

	// メッシュ用定数バッファ
	_meshConstantBuffer.Create(device, sizeof(CbMesh));
	// グラス用定数バッファ
	_grassConstantBuffer.Create(device, sizeof(CbGrass));
}

void GrassShader::Begin(const RenderContext& rc)
{
	ID3D11DeviceContext* dc = rc.deviceContext;

	// シェーダー設定
	dc->IASetInputLayout(_vertexShader.GetInputLayout());
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
	const Material* material)
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
	cbGrass.shakeAxis		= *material->GetParameterI1("shakeAxis");
	cbGrass.shakeAmplitude	= *material->GetParameterF1("shakeAmplitude");
	cbGrass.windSpeed		= *material->GetParameterF1("windSpeed");
	cbGrass.windDirection	= *material->GetParameterF3("windDirection");
	dc->UpdateSubresource(_grassConstantBuffer.Get(), 0, 0, &cbGrass, 0, 0);

	// シェーダーリソースビュー設定
	ID3D11ShaderResourceView* srvs[] =
	{
		material->GetTextureData("Diffuse").Get(),
		material->GetTextureData("Normal").Get(),
		material->GetTextureData("Specular").Get(),
		material->GetTextureData("Roughness").Get(),
		material->GetTextureData("Emissive").Get()
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

Material::ParameterMap GrassShader::GetParameterMap() const
{
	CbGrass cb;
    Material::ParameterMap p;
    p["shakeAxis"] = cb.shakeAxis;
    p["shakeAmplitude"] = cb.shakeAmplitude;
    p["windSpeed"] = cb.windSpeed;
    p["windDirection"] = cb.windDirection;
    return p;
}
