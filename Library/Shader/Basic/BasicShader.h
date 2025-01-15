#pragma once

#include "../ShaderBase.h"

class BasicShader : public ShaderBase
{
public:
    /// <summary>
    /// コンストラクタ
	/// ピクセルシェーダは固定
    /// </summary>
    /// <param name="device"></param>
    /// <param name="vsName"></param>
    /// <param name="inputDescs"></param>
    /// <param name="inputSize"></param>
    BasicShader(ID3D11Device* device,
		const char* vsName,
		D3D11_INPUT_ELEMENT_DESC* inputDescs, UINT inputSize);
    ~BasicShader() override = default;

	// 開始処理
	void Begin(const RenderContext& rc) override;

	// 更新処理
	void Update(const RenderContext& rc,
		const ModelResource::Material* material) override;

	// 終了処理
	void End(const RenderContext& rc) override;

private:
	struct CbMesh
	{
		DirectX::XMFLOAT4		materialColor;
	};

	Microsoft::WRL::ComPtr<ID3D11VertexShader>		vertexShader_;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>		pixelShader_;
	Microsoft::WRL::ComPtr<ID3D11InputLayout>		inputLayout_;
	Microsoft::WRL::ComPtr<ID3D11Buffer>			meshConstantBuffer_;
};