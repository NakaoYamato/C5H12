#pragma once
#include "../ModelShaderBase.h"

class PhongShader : public ModelShaderBase
{
public:
	PhongShader(ID3D11Device* device,
		const char* vsName,
		const char* psName,
		D3D11_INPUT_ELEMENT_DESC* inputDescs, UINT inputSize);
	~PhongShader() override = default;

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
		DirectX::XMFLOAT4		Ka;// アンビエント
		DirectX::XMFLOAT4		Kd;// ディフューズ
		DirectX::XMFLOAT4		Ks;// スペキュラー

		int decalMask = 0;
		float dummy[3];
	};

	VertexShader 	_vertexShader;
	PixelShader		_pixelShader;
	ConstantBuffer	_meshConstantBuffer;
};