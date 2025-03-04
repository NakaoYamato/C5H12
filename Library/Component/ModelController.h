#pragma once

#include "Component.h"
#include "../../Library/Renderer/ModelRenderer.h"

/// <summary>
/// モデルコンポーネント
/// </summary>
class ModelController : public Component
{
public:
	ModelController(const char* filename);
	~ModelController()override {}

	// 名前取得
	const char* GetName()const { return "ModelController"; }

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

	// アクセサ
	Model* GetModel() { return model_.get(); }
	const ShaderBase::Parameter& GetShaderParameter()const { return shaderParameter_; }
	const ShaderBase::Parameter& GetShadowParameter()const { return shadowParameter_; }
	ShaderId GetShaderId()const { return shaderId_; }
	ModelRenderType GetRenderType()const { return renderType_; }
	const Vector4& GetColor()const { return color_; }

	void SetShaderParameter(const ShaderBase::Parameter& parameter) { this->shaderParameter_ = parameter; }
	void SetShadowParameter(const ShaderBase::Parameter& parameter) { this->shadowParameter_ = parameter; }
	void SetShaderId(ShaderId id) { this->shaderId_ = id; }
	void SetRenderType(ModelRenderType type) { this->renderType_ = renderType_; }
	void SetColor(const Vector4& c) { this->color_ = c; }

private:
	std::unique_ptr<Model> model_;
	ShaderBase::Parameter shaderParameter_;
	ShaderBase::Parameter shadowParameter_;
	ShaderId shaderId_ = ShaderId::Phong;
	ModelRenderType renderType_ = ModelRenderType::Dynamic;
	Vector4 color_{ 1,1,1,1 };
};