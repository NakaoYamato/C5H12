#pragma once

#include "../SpriteShaderBase.h"
class SpriteScrollShader : public SpriteShaderBase
{
public:
	struct CBScroll
	{
		float scrollU;	// Uスクロール量
		float scrollV;	// Vスクロール量
		float padding[2];
	};

public:
	SpriteScrollShader(ID3D11Device* device,
		D3D11_INPUT_ELEMENT_DESC* inputDescs, UINT inputSize);
	~SpriteScrollShader() override = default;
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
	ConstantBuffer _cbScroll;
};
