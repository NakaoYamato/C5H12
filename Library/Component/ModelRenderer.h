#pragma once

#include "Component.h"
#include "../../Library/Renderer/MeshRenderer.h"

/// <summary>
/// モデルコンポーネント
/// </summary>
class ModelRenderer : public Component
{
public:
	ModelRenderer() {}
	~ModelRenderer()override {}

	// 名前取得
	const char* GetName()const { return "ModelRenderer"; }

	// 開始処理
	void Start()override {}

	// 更新処理
	void Update(float elapsedTime) override;

	// 1秒ごとの更新処理
	void FixedUpdate() override {}

	// 描画処理
	void Render(const RenderContext& rc) override;

	// 影描画
	void CastShadow(const RenderContext& rc) override;

	// GUI描画
	void DrawGui() override;

#pragma region アクセサ
	const ShaderBase::Parameter& GetShaderParameter()const { return _shaderParameter; }
	const ShaderBase::Parameter& GetShadowParameter()const { return _shadowParameter; }
	ModelRenderType GetRenderType()const { return _renderType; }
	const Vector4& GetColor()const { return _color; }

	void SetShaderParameter(const ShaderBase::Parameter& parameter) { this->_shaderParameter = parameter; }
	void SetShadowParameter(const ShaderBase::Parameter& parameter) { this->_shadowParameter = parameter; }
	void SetShader(std::string name);
	void SetRenderType(ModelRenderType type) { this->_renderType = type; }
	void SetColor(const Vector4& c) { this->_color = c; }
#pragma endregion
private:
	ShaderBase::Parameter _shaderParameter;
	ShaderBase::Parameter _shadowParameter;
	ModelRenderType _renderType = ModelRenderType::Dynamic;
	Vector4 _color{ 1,1,1,1 };
};