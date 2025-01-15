#include "CascadedShadowMapShader.h"
#include "../../HrTrace.h"
#include "../../ResourceManager/GpuResourceManager.h"

CascadedShadowMapShader::CascadedShadowMapShader(ID3D11Device* device, 
    const char* vsName,
    D3D11_INPUT_ELEMENT_DESC* inputDescs, UINT inputSize)
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
	// 必要ない

	// ジオメトリシェーダー
	GpuResourceManager::CreateGsFromCso(device,
		"./Data/Shader/CascadedShadowGS.cso",
		geometryShader_.ReleaseAndGetAddressOf());
}

void CascadedShadowMapShader::Begin(const RenderContext& rc)
{
	ID3D11DeviceContext* dc = rc.deviceContext;

	// シェーダー設定
	dc->IASetInputLayout(inputLayout_.Get());
	dc->VSSetShader(vertexShader_.Get(), nullptr, 0);
	dc->GSSetShader(geometryShader_.Get(), nullptr, 0);
	dc->PSSetShader(nullptr, nullptr, 0);
}

void CascadedShadowMapShader::Update(const RenderContext&, 
	const ModelResource::Material*)
{
	// 特にやることはない
}

void CascadedShadowMapShader::End(const RenderContext& rc)
{
	ID3D11DeviceContext* dc = rc.deviceContext;
	// ジオメトリシェーダー設定解除
	dc->VSSetShader(nullptr, nullptr, 0);
	dc->IASetInputLayout(nullptr);
	dc->GSSetShader(nullptr, nullptr, 0);
}
