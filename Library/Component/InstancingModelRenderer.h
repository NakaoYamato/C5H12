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
	// –¼‘Oæ“¾
	const char* GetName() const { return "InstancingModelRenderer"; }
	// ¶¬ˆ—
	void OnCreate() override;
	// •`‰æˆ—
	void Render(const RenderContext& rc) override;
	// ‰e•`‰æ
	void CastShadow(const RenderContext& rc) override;
	// GUI•`‰æ
	void DrawGui() override;

private:
	std::weak_ptr<Model> _model;
	ShaderBase::Parameter _shaderParameter;
	ModelRenderType _renderType = ModelRenderType::Dynamic;
	std::string _shaderType = "PhongAlpha";
	Vector4 _color{ 1,1,1,1 };
	Material _material;
};