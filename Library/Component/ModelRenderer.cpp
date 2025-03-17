#include "ModelRenderer.h"

#include <imgui.h>

#include "../../Library/Graphics/Graphics.h"

ModelRenderer::ModelRenderer(const char* filename)
{
	_model = std::make_unique<Model>(Graphics::Instance().GetDevice(), filename);
}

// 更新処理
void ModelRenderer::Update(float elapsedTime)
{
	_model->UpdateTransform(GetActor()->GetTransform().GetMatrix());
}

// 描画処理
void ModelRenderer::Render(const RenderContext& rc)
{
	const ModelResource* resource = _model->GetResource();
	const std::vector<ModelResource::Node>& nodes = _model->GetPoseNodes();
	for (const ModelResource::Mesh& mesh : resource->GetMeshes())
	{
		MeshRenderer::Draw(&mesh, _model.get(), _color, _shaderName, _renderType, &_shaderParameter);
	}
}

// 影描画
void ModelRenderer::CastShadow(const RenderContext& rc)
{
	const ModelResource* resource = _model->GetResource();
	const std::vector<ModelResource::Node>& nodes = _model->GetPoseNodes();
	for (const ModelResource::Mesh& mesh : resource->GetMeshes())
	{
		MeshRenderer::Draw(&mesh, _model.get(), _VECTOR4_WHITE, "CascadedShadowMap", _renderType, &_shadowParameter);
	}
}

// GUI描画
void ModelRenderer::DrawGui()
{
	ImGui::ColorEdit4("color", &_color.x);
	ImGui::Text((u8"現在のシェーダー:" + _shaderName).c_str());
	ImGui::Separator();
	auto shaderName = MeshRenderer::GetShaderNames(_renderType, Graphics::Instance().RenderingDeferred());
	if (ImGui::TreeNodeEx(u8"使用可能のシェーダー"))
	{
		for (auto& name : shaderName)
		{
			if (ImGui::TreeNodeEx(name, ImGuiTreeNodeFlags_Leaf))
			{
				// ダブルクリックで変更
				if (ImGui::IsItemClicked())
					SetShader(name);

				ImGui::TreePop();
			}
		}
		ImGui::TreePop();
	}
	ImGui::Separator();
	for (auto& [name, parameter] : _shaderParameter)
	{
		ImGui::DragFloat(name.c_str(), &parameter, 0.1f);
	}
	static const char* renderTypeName[] =
	{
		u8"DynamicBoneModel",
		u8"StaticBoneModel",
	};
	int rId = static_cast<int>(_renderType);
	if (ImGui::Combo(u8"描画タイプ", &rId, renderTypeName, _countof(renderTypeName)))
	{
		// エラー防止のためPhongに変更
		SetShader("Phong");
	}
	_renderType = static_cast<ModelRenderType>(rId);
	_model->DrawGui();
}

void ModelRenderer::SetShader(std::string name)
{
	this->_shaderName = name;
	// パラメータのkye受け取り
	_shaderParameter = MeshRenderer::GetShaderParameterKey(_renderType, _shaderName, Graphics::Instance().RenderingDeferred());
}
