#include "ModelRenderer.h"

#include <imgui.h>

#include "../../Library/Graphics/Graphics.h"

ModelRenderer::ModelRenderer(const char* filename)
{
	model_ = std::make_unique<Model>(Graphics::Instance().GetDevice(), filename);
}

// 更新処理
void ModelRenderer::Update(float elapsedTime)
{
	model_->UpdateTransform(GetActor()->GetTransform().GetMatrix());
}

// 描画処理
void ModelRenderer::Render(const RenderContext& rc)
{
	const ModelResource* resource = model_->GetResource();
	const std::vector<ModelResource::Node>& nodes = model_->GetPoseNodes();
	for (const ModelResource::Mesh& mesh : resource->GetMeshes())
	{
		MeshRenderer::Draw(&mesh, model_.get(), color_, shaderId_, renderType_, &shaderParameter_);
	}
}

// 影描画
void ModelRenderer::CastShadow(const RenderContext& rc)
{
	const ModelResource* resource = model_->GetResource();
	const std::vector<ModelResource::Node>& nodes = model_->GetPoseNodes();
	for (const ModelResource::Mesh& mesh : resource->GetMeshes())
	{
		MeshRenderer::Draw(&mesh, model_.get(), VECTOR4_WHITE, "CascadedShadowMap", renderType_, &shadowParameter_);
	}
}

// GUI描画
void ModelRenderer::DrawGui()
{
	ImGui::ColorEdit4("color", &color_.x);
	ImGui::Text((u8"現在のシェーダー:" + shaderId_).c_str());
	ImGui::Separator();
	auto shaderName = MeshRenderer::GetShaderNames(renderType_);
	if (ImGui::TreeNodeEx(u8"使用可能のシェーダー"))
	{
		for (auto& name : shaderName)
		{
			if (ImGui::TreeNodeEx(name, ImGuiTreeNodeFlags_Leaf))
			{
				// ダブルクリックで変更
				if (ImGui::IsItemClicked())
					shaderId_ = name;

				ImGui::TreePop();
			}
		}
		ImGui::TreePop();
	}
	ImGui::Separator();

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
