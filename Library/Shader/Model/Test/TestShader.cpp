#include "TestShader.h"
#include "../../Graphics/GpuResourceManager.h"

TestShader::TestShader(ID3D11Device* device, const char* vsName, const char* psName, D3D11_INPUT_ELEMENT_DESC* inputDescs, UINT inputSize)
{
	// 頂点シェーダー
	_vertexShader.Load(device, vsName, inputDescs, inputSize);

	// ピクセルシェーダ
	_pixelShader.Load(device, psName);
}

void TestShader::Begin(const RenderContext& rc)
{
	ID3D11DeviceContext* dc = rc.deviceContext;

	// シェーダー設定
	dc->IASetInputLayout(_vertexShader.GetInputLayout());
	dc->VSSetShader(_vertexShader.Get(), nullptr, 0);
	dc->PSSetShader(_pixelShader.Get(), nullptr, 0);
}

void TestShader::Update(const RenderContext& rc, const Material* material)
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

Material::ParameterMap TestShader::GetParameterMap() const
{
	Material::ParameterMap p;
	return p;
}
