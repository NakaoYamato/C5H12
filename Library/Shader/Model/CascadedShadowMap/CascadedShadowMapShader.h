#pragma once
#include "../ModelShaderBase.h"

// カスケードシャドウマップの書き込む先の数
#define _CASCADED_SHADOW_MAPS_SIZE 4

// カスケードシャドウマップを描画するためのシェーダー
class CascadedShadowMapShader : public ModelShaderBase
{
public:
	CascadedShadowMapShader(ID3D11Device* device,
		const char* vsName,
		D3D11_INPUT_ELEMENT_DESC* inputDescs, UINT inputSize);
	~CascadedShadowMapShader() override = default;

	// 開始処理
	void Begin(const RenderContext& rc) override;

	// 更新処理
	void Update(const RenderContext& rc,
		const Material* material) override {
	}

	// 終了処理
	void End(const RenderContext& rc) override;

	// パラメータのkey取得
	Material::ParameterMap GetParameterMap()const override;
private:
	VertexShader 	_vertexShader;
	GeometryShader 	_geometryShader;
};