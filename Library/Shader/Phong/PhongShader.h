#pragma once

#include "../ShaderBase.h"

class PhongShader : public ShaderBase
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
		const ModelResource::Material* material,
		Parameter* parameter) override;

	// 終了処理
	void End(const RenderContext& rc) override;

private:
	struct CbMesh
	{
		DirectX::XMFLOAT4		Ka;// アンビエント
		DirectX::XMFLOAT4		Kd;// ディフューズ
		DirectX::XMFLOAT4		Ks;// スペキュラー
	};

	Microsoft::WRL::ComPtr<ID3D11VertexShader>		vertexShader_;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>		pixelShader_;
	Microsoft::WRL::ComPtr<ID3D11InputLayout>		inputLayout_;
	Microsoft::WRL::ComPtr<ID3D11Buffer>			meshConstantBuffer_;
};