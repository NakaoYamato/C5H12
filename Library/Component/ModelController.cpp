#include "ModelController.h"

#include <imgui.h>

#include "../../Library/Graphics/Graphics.h"
#include "../../Library/Renderer/ModelRenderer.h"

ModelController::ModelController(const char* filename)
{
	model = std::make_unique<Model>(Graphics::Instance().GetDevice(), filename);
}

// 更新処理
void ModelController::Update(float elapsedTime)
{
	model->UpdateTransform(GetActor()->GetTransform().GetMatrix());
}

// 描画処理
void ModelController::Render(const RenderContext& rc)
{
	ModelRenderer::Draw(model.get(), color, shaderId, renderType);
}

// 影描画
void ModelController::CastShadow(const RenderContext& rc)
{
	ModelRenderer::Draw(model.get(), color, ShaderId::CascadedShadowMap, renderType);
}

// GUI描画
void ModelController::DrawGui()
{
	ImGui::ColorEdit4("color", &color.x);
	static const char* shaderTypeName[] =
	{
		u8"Basic",
		u8"Phong",
		u8"Ramp",
	};
	int sId = static_cast<int>(shaderId);
	ImGui::Combo(u8"使用するシェーダタイプ", &sId, shaderTypeName, _countof(shaderTypeName));
	shaderId = static_cast<ShaderId>(sId);
	static const char* renderTypeName[] =
	{
		u8"DynamicBoneModel",
		u8"StaticBoneModel",
	};
	int rId = static_cast<int>(renderType);
	ImGui::Combo(u8"描画タイプ", &rId, renderTypeName, _countof(renderTypeName));
	renderType = static_cast<ModelRenderType>(rId);
	model->DrawGui();
}
