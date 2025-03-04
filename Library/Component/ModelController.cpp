#include "ModelController.h"

#include <imgui.h>

#include "../../Library/Graphics/Graphics.h"
#include "../../Library/Renderer/ModelRenderer.h"

ModelController::ModelController(const char* filename)
{
	model_ = std::make_unique<Model>(Graphics::Instance().GetDevice(), filename);
}

// 更新処理
void ModelController::Update(float elapsedTime)
{
	model_->UpdateTransform(GetActor()->GetTransform().GetMatrix());
}

// 描画処理
void ModelController::Render(const RenderContext& rc)
{
	ModelRenderer::Draw(model_.get(), color_, shaderId_, renderType_, &shaderParameter_);
}

// 影描画
void ModelController::CastShadow(const RenderContext& rc)
{
	ModelRenderer::Draw(model_.get(), VECTOR4_WHITE, ShaderId::CascadedShadowMap, renderType_, &shadowParameter_);
}

// GUI描画
void ModelController::DrawGui()
{
	ImGui::ColorEdit4("color", &color_.x);
	static const char* shaderTypeName[] =
	{
		u8"Phong",
		u8"Ramp",
	};
	int sId = static_cast<int>(shaderId_);
	ImGui::Combo(u8"使用するシェーダタイプ", &sId, shaderTypeName, _countof(shaderTypeName));
	shaderId_ = static_cast<ShaderId>(sId);
	static const char* renderTypeName[] =
	{
		u8"DynamicBoneModel",
		u8"StaticBoneModel",
	};
	int rId = static_cast<int>(renderType_);
	ImGui::Combo(u8"描画タイプ", &rId, renderTypeName, _countof(renderTypeName));
	renderType_ = static_cast<ModelRenderType>(rId);
	model_->DrawGui();
}
