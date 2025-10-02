#include "SimplePrimitiveShader.h"

SimplePrimitiveShader::SimplePrimitiveShader(ID3D11Device* device, D3D11_INPUT_ELEMENT_DESC* inputDescs, UINT inputSize)
{
	// 頂点シェーダー
	_vertexShader.Load(device, "./Data/Shader/HLSL/PrimitiveRenderer/PrimitiveRendererVS.cso", inputDescs, inputSize);
	// ピクセルシェーダ読み込み
	_pixelShader.Load(device, "./Data/Shader/HLSL/PrimitiveRenderer/PrimitiveRendererPS.cso");
}

// 開始処理
void SimplePrimitiveShader::Begin(const RenderContext& rc)
{
	ID3D11DeviceContext* dc = rc.deviceContext;
	// シェーダー設定
	dc->VSSetShader(_vertexShader.Get(), nullptr, 0);
	dc->IASetInputLayout(_vertexShader.GetInputLayout());
	dc->PSSetShader(_pixelShader.Get(), nullptr, 0);
}

// 更新処理
void SimplePrimitiveShader::Update(const RenderContext& rc, const Material* material)
{
}

// 終了処理
void SimplePrimitiveShader::End(const RenderContext& rc)
{
	ID3D11DeviceContext* dc = rc.deviceContext;
	// シェーダ解除
	dc->VSSetShader(NULL, 0, 0);
	dc->PSSetShader(NULL, 0, 0);
}

// パラメータのkey取得
Material::ParameterMap SimplePrimitiveShader::GetParameterMap() const
{
	return Material::ParameterMap();
}
