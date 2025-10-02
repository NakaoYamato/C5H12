#pragma once

#include "Component.h"
#include "../../Library/Renderer/MeshRenderer.h"

class InstancingModelRenderer : public Component
{
public:
	InstancingModelRenderer(std::shared_ptr<Model> model) :
		_model(model)
	{}
	~InstancingModelRenderer() override {}
	// 名前取得
	const char* GetName() const { return "InstancingModelRenderer"; }
	// 生成時処理
	void OnCreate() override;
	// 描画処理
	void Render(const RenderContext& rc) override;
	// 影描画
	void CastShadow(const RenderContext& rc) override;
	// GUI描画
	void DrawGui() override;

	// シェーダー変更
	void ChangeShader(const std::string& shaderName);
#pragma region アクセサ
	// 色取得
	const Vector4& GetColor() const { return _color; }
	// 色設定
	void SetColor(const Vector4& color) { _color = color; }
	// マテリアル取得
	Material& GetMaterial() { return _material; }
	// マテリアル設定
	void SetMaterial(const Material& material) { _material = material; }
#pragma endregion
private:
	std::weak_ptr<Model> _model;
	Vector4 _color{ 1,1,1,1 };
	Material _material;
};