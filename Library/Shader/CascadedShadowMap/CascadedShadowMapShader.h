#pragma once
#include "../ShaderBase.h"

// カスケードシャドウマップの書き込む先の数
#define CASCADED_SHADOW_MAPS_SIZE 3

// カスケードシャドウマップを描画するためのシェーダー
class CascadedShadowMapShader : public ShaderBase
{
public:
	CascadedShadowMapShader(ID3D11Device * device,
		const char* vsName,
		D3D11_INPUT_ELEMENT_DESC * inputDescs, UINT inputSize);
	~CascadedShadowMapShader() override = default;

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
	Microsoft::WRL::ComPtr<ID3D11VertexShader>		_vertexShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout>		_inputLayout;
	Microsoft::WRL::ComPtr<ID3D11GeometryShader>	_geometryShader;
};