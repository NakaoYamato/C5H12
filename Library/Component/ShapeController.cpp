#include "ShapeController.h"

#include <imgui.h>

#include "../../Library/Renderer/ShapeRenderer.h"

void ShapeController::Render(const RenderContext& rc)
{
	switch (_type)
	{
	case ShapeType::Box: ShapeRenderer::DrawBox(GetActor()->GetTransform().GetMatrix(), _color); break;
	case ShapeType::Sphere: ShapeRenderer::DrawSphere(GetActor()->GetTransform().GetWorldPosition(), _radius, _color); break;
	case ShapeType::Capsule: ShapeRenderer::DrawCapsule(GetActor()->GetTransform().GetMatrix(), _radius, _height, _color); break;
	}
}

// 影描画
void ShapeController::CastShadow(const RenderContext& rc)
{
}

void ShapeController::DrawGui()
{
	static const char* shapTypeName[] =
	{
		u8"Box",
		u8"Sphere",
		u8"Capsule",
	};
	int type = static_cast<int>(_type);
	ImGui::Combo(u8"使用するシェーダタイプ", &type, shapTypeName, _countof(shapTypeName));
	_type = static_cast<ShapeType>(type);
	ImGui::DragFloat("radius", &_radius, 0.01f);
	ImGui::DragFloat("height", &_height, 0.01f);
	ImGui::ColorEdit4("color", &_color.x);
}
