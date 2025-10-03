#pragma once
#include "../PrimitiveShaderBase.h"
#include "../../Library/Graphics/ConstantBuffer.h"

class LocusPrimitiveShader : public PrimitiveShaderBase
{
public:
	struct CbLocus
	{
		float distanceRate = 0.1f;
		float colorMinValue = 0.9f;
		float colorMaxValue = 0.9f;
		float locusPadding = {};

		Vector4 baseColor = Vector4::White;
	};

	static constexpr UINT CbLocusIndex = 2;
public:
	LocusPrimitiveShader(ID3D11Device* device, D3D11_INPUT_ELEMENT_DESC* inputDescs, UINT inputSize);
	~LocusPrimitiveShader() override = default;
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
	ConstantBuffer _cbLocus;
};