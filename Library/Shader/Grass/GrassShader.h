#pragma once

#include "../ShaderBase.h"

class GrassShader : public ShaderBase
{
public:
	GrassShader(ID3D11Device* device,
		const char* vsName,
		const char* psName,
		D3D11_INPUT_ELEMENT_DESC* inputDescs, UINT inputSize);
	~GrassShader() override = default;

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
		DirectX::XMFLOAT4		Ka{};// アンビエント
		DirectX::XMFLOAT4		Kd{};// ディフューズ
		DirectX::XMFLOAT4		Ks{};// スペキュラー
	};
	struct CbGrass
	{
		// 0 : +X軸, 1 : +Y軸, 2 : +Z軸
		// 3 : -X軸, 4 : -Y軸, 5 : -Z軸
		int						shakeAxis = 0; // 揺らす軸
		Vector3					windDirection = { 0.0f, 1.0f, 0.0f }; // 風の方向

		float					windSpeed = 1.5f; // 風の速度
		float					shakeAmplitude = 2.0f; // 揺らす振幅
		Vector2					padding{};
	};

	Microsoft::WRL::ComPtr<ID3D11VertexShader>		_vertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>		_pixelShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout>		_inputLayout;
	Microsoft::WRL::ComPtr<ID3D11Buffer>			_meshConstantBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer>			_grassConstantBuffer;
};