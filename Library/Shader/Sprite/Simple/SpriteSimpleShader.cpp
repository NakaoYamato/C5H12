#include "SpriteSimpleShader.h"
#include "../../Graphics/GpuResourceManager.h"

SpriteSimpleShader::SpriteSimpleShader(ID3D11Device* device,
	D3D11_INPUT_ELEMENT_DESC* inputDescs, 
	UINT inputSize)
{
	// 頂点シェーダー
	_vertexShader.Load(device, "./Data/Shader/HLSL/Sprite/SpriteVS.cso", inputDescs, inputSize);

	// ピクセルシェーダ
	_pixelShader.Load(device, "./Data/Shader/HLSL/Sprite/SpritePS.cso");
}

// 開始処理
void SpriteSimpleShader::Begin(const RenderContext& rc)
{
	ID3D11DeviceContext* dc = rc.deviceContext;
	// シェーダー設定
	dc->IASetInputLayout(_vertexShader.GetInputLayout());
	dc->VSSetShader(_vertexShader.Get(), nullptr, 0);
	dc->PSSetShader(_pixelShader.Get(), nullptr, 0);
}

// 更新処理
void SpriteSimpleShader::Update(const RenderContext& rc, const Material* material)
{
}

// 終了処理
void SpriteSimpleShader::End(const RenderContext& rc)
{
	SpriteShaderBase::End(rc);
}

Material::ParameterMap SpriteSimpleShader::GetParameterMap() const
{
	return Material::ParameterMap();
}
