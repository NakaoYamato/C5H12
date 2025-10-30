#pragma once
#include "../ModelShaderBase.h"

class PlayerShader : public ModelShaderBase
{
public:
	PlayerShader(ID3D11Device* device,
		D3D11_INPUT_ELEMENT_DESC* inputDescs, UINT inputSize);
	~PlayerShader() override = default;

	// 開始処理
	void Begin(const RenderContext& rc) override;

	// 更新処理
	void Update(const RenderContext& rc,
		const Material* material) override;

	// 終了処理
	void End(const RenderContext& rc) override;

	// パラメータのkey取得
	Material::ParameterMap GetParameterMap()const override;
private:
	struct CbMesh
	{
		DirectX::XMFLOAT4 baseColor;

		float roughness;
		float metalness;
		int decalMask = 0;
		float dummy;
	};
	struct CbPlayer
	{
		DirectX::XMFLOAT4 bodyColor;
	};

	VertexShader 	_vertexShader;
	PixelShader		_pixelShader;
	ConstantBuffer	_meshConstantBuffer;
	ConstantBuffer	_playerConstantBuffer;
};