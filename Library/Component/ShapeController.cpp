#include "ShapeController.h"

#include <imgui.h>

#include "../../Library/Renderer/ShapeRenderer.h"

void ShapeController::Render(const RenderContext& rc)
{
	switch (type_)
	{
	case ShapeType::Box: ShapeRenderer::DrawBox(GetActor()->GetTransform().GetMatrix(), color_); break;
	case ShapeType::Sphere: ShapeRenderer::DrawSphere(GetActor()->GetTransform().GetWorldPosition(), radius_, color_); break;
	case ShapeType::Capsule: ShapeRenderer::DrawCapsule(GetActor()->GetTransform().GetMatrix(), radius_, height_, color_); break;
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
	int type = static_cast<int>(type_);
	ImGui::Combo(u8"使用するシェーダタイプ", &type, shapTypeName, _countof(shapTypeName));
	type_ = static_cast<ShapeType>(type);
	ImGui::DragFloat("radius", &radius_, 0.01f);
	ImGui::DragFloat("height", &height_, 0.01f);
	ImGui::ColorEdit4("color", &color_.x);
}
