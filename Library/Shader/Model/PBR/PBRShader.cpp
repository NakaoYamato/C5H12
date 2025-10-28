#include "PBRShader.h"
#include "../../Graphics/GpuResourceManager.h"

PBRShader::PBRShader(ID3D11Device* device,
    const char* vsName,
    const char* psName, 
    D3D11_INPUT_ELEMENT_DESC* inputDescs,
    UINT inputSize)
{
	// 頂点シェーダー
	_vertexShader.Load(device, vsName, inputDescs, inputSize);

	// ピクセルシェーダ
	_pixelShader.Load(device, psName);


	// メッシュ用定数バッファ
	_meshConstantBuffer.Create(device, sizeof(CbMesh));
}

void PBRShader::Begin(const RenderContext& rc)
{
	ID3D11DeviceContext* dc = rc.deviceContext;

	// シェーダー設定
	dc->IASetInputLayout(_vertexShader.GetInputLayout());
	dc->VSSetShader(_vertexShader.Get(), nullptr, 0);
	dc->PSSetShader(_pixelShader.Get(), nullptr, 0);

	// 定数バッファ設定
	ID3D11Buffer* cbs[] =
	{
		_meshConstantBuffer.Get(),
	};
	dc->PSSetConstantBuffers(CBIndex, _countof(cbs), cbs);
}

void PBRShader::Update(const RenderContext& rc, 
	const Material* material)
{
	ID3D11DeviceContext* dc = rc.deviceContext;

	Vector4 pbrFactor = material->GetColor("PBRFactor");
	// メッシュ用定数バッファ更新
	CbMesh cbMesh{};
	cbMesh.baseColor = material->GetColor("Diffuse");
	cbMesh.roughness = pbrFactor.y;
	if (auto it = material->GetParameterF1("roughnessFactor"))
		cbMesh.roughness *= *it;
	cbMesh.metalness = pbrFactor.z;
	if (auto it = material->GetParameterF1("metalnessFactor"))
		cbMesh.metalness *= *it;
	if (auto value = material->GetParameterI1("decalMask"))
		cbMesh.decalMask = *value;
	dc->UpdateSubresource(_meshConstantBuffer.Get(), 0, 0, &cbMesh, 0, 0);

	// シェーダーリソースビュー設定
	ID3D11ShaderResourceView* srvs[] =
	{
		material->GetTextureData("Diffuse").Get(),
		material->GetTextureData("Roughness").Get(),
		material->GetTextureData("Normal").Get(),
		material->GetTextureData("Emissive").Get()
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

Material::ParameterMap PBRShader::GetParameterMap() const
{
	Material::ParameterMap p;
	p["roughnessFactor"] = 1.0f;
	p["metalnessFactor"] = 1.0f;
	p["decalMask"] = 0;
    return p;
}
