#include "SpriteOutlineShader.h"
#include "../../Graphics/GpuResourceManager.h"

SpriteOutlineShader::SpriteOutlineShader(ID3D11Device* device, D3D11_INPUT_ELEMENT_DESC* inputDescs, UINT inputSize)
{
	// 頂点シェーダー
	_vertexShader.Load(device, "./Data/Shader/HLSL/Sprite/SpriteVS.cso", inputDescs, inputSize);

	// ピクセルシェーダ
	_pixelShader.Load(device, "./Data/Shader/HLSL/Sprite/SpriteOutlinePS.cso");

	// 定数バッファ
	_cbOutline.Create(device, sizeof(CBOutline));
}

// 開始処理
void SpriteOutlineShader::Begin(const RenderContext& rc)
{
	ID3D11DeviceContext* dc = rc.deviceContext;
	// シェーダー設定
	dc->IASetInputLayout(_vertexShader.GetInputLayout());
	dc->VSSetShader(_vertexShader.Get(), nullptr, 0);
	dc->PSSetShader(_pixelShader.Get(), nullptr, 0);

	// 定数バッファ設定
	ID3D11Buffer* cbs[] =
	{
		_cbOutline.Get(),
	};
	dc->PSSetConstantBuffers(CBIndex, _countof(cbs), cbs);
}

// 更新処理
void SpriteOutlineShader::Update(const RenderContext& rc, const Material* material)
{
	// 定数バッファ更新
	CBOutline scroll{};
	if (material != nullptr)
	{
		if (auto param = material->GetParameterF4("outlineColor"))
		{
			scroll.outlineColor = *param;
		}
		if (auto param = material->GetParameterF2("outlineScale"))
		{
			scroll.outlineScale = *param;
		}
		if (auto param = material->GetParameterF1("intensity"))
		{
			scroll.intensity = *param;
		}
	}
	_cbOutline.Update(rc.deviceContext, &scroll);
}

// 終了処理
void SpriteOutlineShader::End(const RenderContext& rc)
{
	SpriteShaderBase::End(rc);
}

// パラメータのkey取得
Material::ParameterMap SpriteOutlineShader::GetParameterMap() const
{
	Material::ParameterMap p;
	p["outlineColor"] = Vector4::Black;
	p["outlineScale"] = Vector2::One;
	p["intensity"] = 1.0f;
	return p;
}
