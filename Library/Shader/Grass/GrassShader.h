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
		const ModelResource::Material* material,
		Parameter* parameter) override;

	// 終了処理
	void End(const RenderContext& rc) override;

	// パラメータのkey取得
	Parameter GetParameterKey()const override;
private:
	struct CbMesh
	{
		float tesselationMaxSubdivision = 5.0f;
		float bladeHeight = 3.0f;
		float bladeWidth = 0.15f;
		float seed = 0.165f;

		float lodDistanceMax = 80.0f;
		float totalElapsedTime = 0.0f;
		float windDirectionX = 1.0f;
		float windDirectionZ = 0.0f;
	};

	Microsoft::WRL::ComPtr<ID3D11VertexShader>		vertexShader_;
	Microsoft::WRL::ComPtr<ID3D11HullShader>		hullShader;
	Microsoft::WRL::ComPtr<ID3D11DomainShader>		domainShader;
	Microsoft::WRL::ComPtr<ID3D11GeometryShader>	geometryShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>		pixelShader_;
	Microsoft::WRL::ComPtr<ID3D11InputLayout>		inputLayout_;
	Microsoft::WRL::ComPtr<ID3D11Buffer>			meshConstantBuffer_;
};