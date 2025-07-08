#include "InstancingModelRenderer.h"

#include <imgui.h>

#include "../../Library/Scene/Scene.h"
#include "../../Library/Graphics/Graphics.h"

// ¶¬Žžˆ—
void InstancingModelRenderer::OnCreate()
{
	_material = _model.lock()->GetMaterials().at(0);
}
// •`‰æˆ—
void InstancingModelRenderer::Render(const RenderContext& rc)
{
	GetActor()->GetScene()->GetMeshRenderer().DrawInstancing(
		_model.lock().get(),
		_color,
		&_material,
		_shaderType,
		GetActor()->GetTransform().GetMatrix(),
		&_shaderParameter);
}
// ‰e•`‰æ
void InstancingModelRenderer::CastShadow(const RenderContext& rc)
{
}
// GUI•`‰æ
void InstancingModelRenderer::DrawGui()
{
	ImGui::ColorEdit4("color", &_color.x);
	ImGui::Separator();
}
