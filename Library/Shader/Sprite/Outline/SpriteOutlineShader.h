#pragma once

#include "../SpriteShaderBase.h"
class SpriteOutlineShader : public SpriteShaderBase
{
public:
	struct CBOutline
	{
		Vector4 outlineColor = Vector4::Black;
		Vector2	outlineScale = Vector2::One;
		float intensity = 1.0f;
		float padding = 0.0f;
	};

public:
	SpriteOutlineShader(ID3D11Device* device,
		D3D11_INPUT_ELEMENT_DESC* inputDescs, UINT inputSize);
	~SpriteOutlineShader() override = default;
	// 開始処理
	void Begin(const RenderContext& rc) override;
	// 更新処理
	void Update(const RenderContext& rc, const Material* material) override;
	// 終了処理
	void End(const RenderContext& rc) override;
	// パラメータのkey取得
	Material::ParameterMap GetParameterMap() const override;
private:
	VertexShader 	_vertexShader;
	PixelShader		_pixelShader;
	ConstantBuffer _cbOutline;
};