#include "GrassShader.h"
#include "../../HRTrace.h"
#include "../../ResourceManager/GpuResourceManager.h"

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

	// ハルシェーダー作成
	GpuResourceManager::CreateHsFromCso(device, "./Data/Shader/GrassHS.cso", _hullShader.ReleaseAndGetAddressOf());

	// ドメインシェーダー作成
	GpuResourceManager::CreateDsFromCso(device, "./Data/Shader/GrassDS.cso", _domainShader.ReleaseAndGetAddressOf());
	
	// ジオメトリシェーダー作成
	GpuResourceManager::CreateGsFromCso(device, "./Data/Shader/GrassGS.cso", _geometryShader.ReleaseAndGetAddressOf());

	// ピクセルシェーダ
	GpuResourceManager::CreatePsFromCso(device, psName,	_pixelShader.ReleaseAndGetAddressOf());

	// メッシュ用定数バッファ
	(void)GpuResourceManager::CreateConstantBuffer(device,
		sizeof(CbMesh),
		_meshConstantBuffer.ReleaseAndGetAddressOf());
}

void GrassShader::Begin(const RenderContext& rc)
{
	ID3D11DeviceContext* dc = rc.deviceContext;

	// シェーダー設定
	dc->IASetInputLayout(_inputLayout.Get());
	dc->VSSetShader(_vertexShader.Get(),	nullptr, 0);
	dc->HSSetShader(_hullShader.Get(),		nullptr, 0);
	dc->DSSetShader(_domainShader.Get(),		nullptr, 0);
	dc->GSSetShader(_geometryShader.Get(),	nullptr, 0);
	dc->PSSetShader(_pixelShader.Get(),		nullptr, 0);

	// 定数バッファ設定
	ID3D11Buffer* cbs[] =
	{
		_meshConstantBuffer.Get(),
	};
	dc->HSSetConstantBuffers(CBIndex, _countof(cbs), cbs);
	dc->GSSetConstantBuffers(CBIndex, _countof(cbs), cbs);
}

void GrassShader::Update(const RenderContext& rc, 
    const ModelResource::Material* material, 
    Parameter* parameter)
{
	ID3D11DeviceContext* dc = rc.deviceContext;

	// メッシュ用定数バッファ更新
	CbMesh cbMesh{};
	cbMesh.tesselationMaxSubdivision	= (*parameter)["tesselationMaxSubdivision"];
	cbMesh.bladeHeight					= (*parameter)["bladeHeight"];
	cbMesh.bladeWidth					= (*parameter)["bladeWidth"];
	cbMesh.seed							= (*parameter)["seed"];
	cbMesh.lodDistanceMax				= (*parameter)["lodDistanceMax"];
	cbMesh.totalElapsedTime				= (*parameter)["totalElapsedTime"];
	cbMesh.windDirectionX				= (*parameter)["windDirectionX"];
	cbMesh.windDirectionZ				= (*parameter)["windDirectionZ"];
	dc->UpdateSubresource(_meshConstantBuffer.Get(), 0, 0, &cbMesh, 0, 0);

	dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST);
}

void GrassShader::End(const RenderContext& rc)
{
	ID3D11DeviceContext* dc = rc.deviceContext;

	// シェーダー設定解除
	dc->VSSetShader(nullptr, nullptr, 0);
	dc->HSSetShader(nullptr, nullptr, 0);
	dc->DSSetShader(nullptr, nullptr, 0);
	dc->GSSetShader(nullptr, nullptr, 0);
	dc->PSSetShader(nullptr, nullptr, 0);
	dc->IASetInputLayout(nullptr);

	// 定数バッファ設定解除
	ID3D11Buffer* cbs[] = { nullptr };
	dc->HSSetConstantBuffers(CBIndex, _countof(cbs), cbs);
	dc->GSSetConstantBuffers(CBIndex, _countof(cbs), cbs);
}

ShaderBase::Parameter GrassShader::GetParameterKey() const
{
	CbMesh cb;
    ShaderBase::Parameter p;
    p["tesselationMaxSubdivision"] = cb.tesselationMaxSubdivision;
    p["bladeHeight"] = cb.bladeHeight;
    p["bladeWidth"] = cb.bladeWidth;
    p["seed"] = cb.seed;
    p["lodDistanceMax"] = cb.lodDistanceMax;
    p["totalElapsedTime"] = cb.totalElapsedTime;
    p["windDirectionX"] = cb.windDirectionX;
    p["windDirectionZ"] = cb.windDirectionZ;
    return p;
}
