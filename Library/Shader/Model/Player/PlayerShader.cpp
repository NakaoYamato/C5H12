#include "PlayerShader.h"
#include "../../Graphics/GpuResourceManager.h"

PlayerShader::PlayerShader(ID3D11Device* device, D3D11_INPUT_ELEMENT_DESC* inputDescs, UINT inputSize)
{
	// 頂点シェーダー
	_vertexShader.Load(device, "./Data/Shader/HLSL/Model/Phong/PhongVS.cso", inputDescs, inputSize);

	// ピクセルシェーダ
	_pixelShader.Load(device, "./Data/Shader/HLSL/Model/Player/PlayerGBPS.cso");

	// 定数バッファ
	_meshConstantBuffer.Create(device, sizeof(CbMesh));
	_playerConstantBuffer.Create(device, sizeof(CbPlayer));
}

// 開始処理
void PlayerShader::Begin(const RenderContext& rc)
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
	dc->PSSetConstantBuffers(4, 1, _playerConstantBuffer.GetAddressOf());
}

// 更新処理
void PlayerShader::Update(const RenderContext& rc, const Material* material)
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
	CbPlayer cbPlayer{};
	if (auto value = material->GetParameterF4("bodyColor"))
		cbPlayer.bodyColor = *value;
	dc->UpdateSubresource(_meshConstantBuffer.Get(), 0, 0, &cbMesh, 0, 0);
	dc->UpdateSubresource(_playerConstantBuffer.Get(), 0, 0, &cbPlayer, 0, 0);

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

// 終了処理
void PlayerShader::End(const RenderContext& rc)
{
	ID3D11DeviceContext* dc = rc.deviceContext;

	// シェーダー設定解除
	dc->VSSetShader(nullptr, nullptr, 0);
	dc->PSSetShader(nullptr, nullptr, 0);
	dc->IASetInputLayout(nullptr);

	// 定数バッファ設定解除
	ID3D11Buffer* cbs[] = { nullptr };
	dc->PSSetConstantBuffers(CBIndex, _countof(cbs), cbs);
	dc->PSSetConstantBuffers(4, 1, cbs);

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

// パラメータのkey取得
Material::ParameterMap PlayerShader::GetParameterMap() const
{
	Material::ParameterMap p;
	p["roughnessFactor"] = 1.0f;
	p["metalnessFactor"] = 1.0f;
	p["decalMask"] = 0;
	p["bodyColor"] = Vector4::Red;
	return p;
}
