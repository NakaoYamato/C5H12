#pragma once

#include "../ShaderBase.h"

class PBRShader : public ShaderBase
{
public:
	PBRShader(ID3D11Device* device,
		const char* vsName,
		const char* psName,
		D3D11_INPUT_ELEMENT_DESC* inputDescs, UINT inputSize);
	~PBRShader() override = default;

	// 開始処理
	void Begin(const RenderContext& rc) override;

	// 更新処理
	void Update(const RenderContext& rc,
		const ModelResource::Material* material,
		Parameter* parameter) override;

	// 終了処理
	void End(const RenderContext& rc) override;

	// パラメータのkey取得
	Parameter GetParameterKey()const override;
private:
	struct CbMesh
	{
		DirectX::XMFLOAT4 baseColor;

		float roughness;
		float metalness;
		float dummy[2];
	};

	Microsoft::WRL::ComPtr<ID3D11VertexShader>		_vertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>		_pixelShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout>		_inputLayout;
	Microsoft::WRL::ComPtr<ID3D11Buffer>			_meshConstantBuffer;
};