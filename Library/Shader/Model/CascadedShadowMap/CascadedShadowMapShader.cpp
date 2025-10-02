#include "CascadedShadowMapShader.h"
#include "../../HrTrace.h"
#include "../../Graphics/GpuResourceManager.h"

CascadedShadowMapShader::CascadedShadowMapShader(ID3D11Device* device, 
    const char* vsName,
    D3D11_INPUT_ELEMENT_DESC* inputDescs, UINT inputSize)
{
	// 頂点シェーダー
	_vertexShader.Load(device, vsName, inputDescs, inputSize);

	// ピクセルシェーダ
	// 必要ない

	// ジオメトリシェーダー
	_geometryShader.Load(device,
		"./Data/Shader/HLSL/Model/CascadedShadow/CascadedShadowGS.cso");
}

void CascadedShadowMapShader::Begin(const RenderContext& rc)
{
	ID3D11DeviceContext* dc = rc.deviceContext;

	// シェーダー設定
	dc->IASetInputLayout(_vertexShader.GetInputLayout());
	dc->VSSetShader(_vertexShader.Get(), nullptr, 0);
	dc->GSSetShader(_geometryShader.Get(), nullptr, 0);
	dc->PSSetShader(nullptr, nullptr, 0);
}

void CascadedShadowMapShader::End(const RenderContext& rc)
{
	ID3D11DeviceContext* dc = rc.deviceContext;
	// ジオメトリシェーダー設定解除
	dc->VSSetShader(nullptr, nullptr, 0);
	dc->IASetInputLayout(nullptr);
	dc->GSSetShader(nullptr, nullptr, 0);
}

Material::ParameterMap CascadedShadowMapShader::GetParameterMap() const
{
	return Material::ParameterMap();
}
