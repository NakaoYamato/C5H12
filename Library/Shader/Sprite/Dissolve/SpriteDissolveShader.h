#pragma once

#include "../SpriteShaderBase.h"
class SpriteDissolveShader : public SpriteShaderBase
{
public:
	struct CBDissolve
	{
		float amount = 0.0f;
		float borderWidth = 0.1f;
		Vector2 maskScroll = Vector2::Zero;
		Vector4 borderColor = Vector4::Black;
	};
public:
	SpriteDissolveShader(ID3D11Device* device,
		D3D11_INPUT_ELEMENT_DESC* inputDescs, UINT inputSize);
	~SpriteDissolveShader() override = default;
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
	ConstantBuffer _cbDissolve;
	Texture _maskTexture;
};