#include "TestShader.h"
#include "../../Graphics/GpuResourceManager.h"

TestShader::TestShader(ID3D11Device* device, const char* vsName, const char* psName, D3D11_INPUT_ELEMENT_DESC* inputDescs, UINT inputSize)
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
}

void TestShader::Begin(const RenderContext& rc)
{
	ID3D11DeviceContext* dc = rc.deviceContext;

	// シェーダー設定
	dc->IASetInputLayout(_inputLayout.Get());
	dc->VSSetShader(_vertexShader.Get(), nullptr, 0);
	dc->PSSetShader(_pixelShader.Get(), nullptr, 0);
}

void TestShader::Update(const RenderContext& rc, const Material* material, Parameter* parameter)
{
}

void TestShader::End(const RenderContext& rc)
{
	ID3D11DeviceContext* dc = rc.deviceContext;

	// シェーダー設定解除
	dc->VSSetShader(nullptr, nullptr, 0);
	dc->PSSetShader(nullptr, nullptr, 0);
	dc->IASetInputLayout(nullptr);
}

ShaderBase::Parameter TestShader::GetParameterKey() const
{
	ShaderBase::Parameter p;
	return p;
}
