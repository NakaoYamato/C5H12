#include "ModelRenderer.h"

#include <imgui.h>

#include "../../Library/Scene/Scene.h"
#include "../../Library/Graphics/Graphics.h"

// 更新処理
void ModelRenderer::Update(float elapsedTime)
{
	auto model = GetActor()->GetModel().lock();
	if (model == nullptr) return;
	model->UpdateTransform(GetActor()->GetTransform().GetMatrix());
}

// 描画処理
void ModelRenderer::Render(const RenderContext& rc)
{
	auto model = GetActor()->GetModel().lock();
	if (model == nullptr) return;

	auto& materialMap = model->GetMaterials();
	const ModelResource* resource = model->GetResource();
	for (const ModelResource::Mesh& mesh : resource->GetMeshes())
	{
		GetActor()->GetScene()->GetMeshRenderer().Draw(
			&mesh,
			model.get(), 
			_color, 
			&materialMap.at(mesh.materialIndex),
			_renderType, 
			&_shaderParameter);
	}
}

// 影描画
void ModelRenderer::CastShadow(const RenderContext& rc)
{
	auto model = GetActor()->GetModel().lock();
	if (model == nullptr) return;

	auto& materialMap = model->GetMaterials();
	const ModelResource* resource = model->GetResource();
	for (const ModelResource::Mesh& mesh : resource->GetMeshes())
	{
		GetActor()->GetScene()->GetMeshRenderer().DrawShadow(
			&mesh, 
			model.get(), 
			Vector4::White,
			&materialMap.at(mesh.materialIndex),
			_renderType, 
			&_shadowParameter);
	}
}

// GUI描画
void ModelRenderer::DrawGui()
{
	ImGui::ColorEdit4("color", &_color.x);
	ImGui::Separator();
	auto shaderName =
		GetActor()->GetScene()->GetMeshRenderer().GetShaderNames(
			_renderType, Graphics::Instance().RenderingDeferred());
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
		_renderType = static_cast<ModelRenderType>(rId);
		// エラー防止のためPhongに変更
		SetShader("Phong");
	}

	auto model = GetActor()->GetModel().lock();
	if (model == nullptr) return;
	model->DrawGui();
}

void ModelRenderer::SetShader(std::string name)
{
	//this->_shaderName = name;
	//// パラメータのkye受け取り
	//_shaderParameter = GetActor()->GetScene()->GetMeshRenderer().GetShaderParameterKey(
	//	_renderType, 
	//	_shaderName,
	//	Graphics::Instance().RenderingDeferred());
}

