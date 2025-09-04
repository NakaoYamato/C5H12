#pragma once

#include "../ShaderBase.h"

class RampShader : public ShaderBase
{
public:
	RampShader(ID3D11Device* device,
		const char* vsName,
		D3D11_INPUT_ELEMENT_DESC* inputDescs, UINT inputSize);
	~RampShader() override = default;

	// 開始処理
	void Begin(const RenderContext& rc) override;

	// 更新処理
	void Update(const RenderContext& rc,
		const Material* material,
		Parameter* parameter) override;

	// 終了処理
	void End(const RenderContext& rc) override;

	// パラメータのkey取得
	Parameter GetParameterKey()const override;
private:
	struct CbMesh
	{
		DirectX::XMFLOAT4		Ka;// アンビエント
		DirectX::XMFLOAT4		Kd;// ディフューズ
		DirectX::XMFLOAT4		Ks;// スペキュラー
	};

	Microsoft::WRL::ComPtr<ID3D11VertexShader>		_vertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>		_pixelShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout>		_inputLayout;
	Microsoft::WRL::ComPtr<ID3D11Buffer>			_meshConstantBuffer;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> _rampSRV;
};