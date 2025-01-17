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
	Model* GetModel() { return model.get(); }
	ShaderId GetShaderId()const { return shaderId; }
	ModelRenderType GetRenderType()const { return renderType; }
	const Vector4& GetColor()const { return color; }

	void SetShaderId(ShaderId id) { this->shaderId = id; }
	void SetRenderType(ModelRenderType type) { this->renderType = renderType; }
	void SetColor(const Vector4& c) { this->color = c; }

private:
	std::unique_ptr<Model> model;
	ShaderId shaderId = ShaderId::Basic;
	ModelRenderType renderType = ModelRenderType::Dynamic;
	Vector4 color{ 1,1,1,1 };
};