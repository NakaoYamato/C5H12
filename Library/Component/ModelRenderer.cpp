#include "ModelRenderer.h"

#include <imgui.h>

#include "../../Library/Scene/Scene.h"
#include "../../Library/Graphics/Graphics.h"
#include "../../Library/Algorithm/Converter.h"

// 生成時処理
void ModelRenderer::OnCreate()
{
	if (GetActor()->GetModel().lock() && !_model.lock().get())
	{
		// モデルが存在するならセット
		SetModel(GetActor()->GetModel());
	}
}
// 描画処理
void ModelRenderer::Render(const RenderContext& rc)
{
	if (_model.lock() == nullptr) return;

	const ModelResource* resource = _model.lock()->GetResource();
	for (const ModelResource::Mesh& mesh : resource->GetMeshes())
	{
		GetActor()->GetScene()->GetMeshRenderer().Draw(
			&mesh,
			_model.lock().get(),
			_color, 
			&_materialMap.at(mesh.materialIndex),
			_renderType, 
			&_shaderParameter);
	}
}
// 影描画
void ModelRenderer::CastShadow(const RenderContext& rc)
{
	if (_model.lock() == nullptr) return;

	const ModelResource* resource = _model.lock()->GetResource();
	for (const ModelResource::Mesh& mesh : resource->GetMeshes())
	{
		GetActor()->GetScene()->GetMeshRenderer().DrawShadow(
			&mesh, 
			_model.lock().get(),
			Vector4::White,
			&_materialMap.at(mesh.materialIndex),
			_renderType, 
			&_shadowParameter);
	}
}
// GUI描画
void ModelRenderer::DrawGui()
{
	static const char* renderTypeName[] =
	{
		u8"DynamicBoneModel",
		u8"StaticBoneModel",
	};

	ImGui::ColorEdit4("color", &_color.x);
	ImGui::Separator();
	int rId = static_cast<int>(_renderType);
	if (ImGui::Combo(u8"描画タイプ", &rId, renderTypeName, _countof(renderTypeName)))
	{
		_renderType = static_cast<ModelRenderType>(rId);
	}
	ImGui::Separator();
	if (ImGui::TreeNode(u8"マテリアル"))
	{
		for (auto& material : _materialMap)
		{
			// 使用可能なシェーダー取得
			auto activeShaderTypes =
				GetActor()->GetScene()->GetMeshRenderer().GetShaderNames(_renderType,
					Graphics::Instance().RenderingDeferred() && material.GetBlendType() != BlendType::Alpha);
			if (ImGui::TreeNode(material.GetName().c_str()))
			{
				// シェーダー変更GUI
				if (ImGui::TreeNode(u8"シェーダー変更"))
				{
					auto shaderType = material.GetShaderName();
					for (auto& activeShaderType : activeShaderTypes)
					{
						bool active = activeShaderType == shaderType;
						if (ImGui::RadioButton(activeShaderType, active))
						{
							material.SetShaderName(activeShaderType);
						}
					}
					ImGui::TreePop();
				}
				ImGui::Separator();

				// マテリアルのGUI描画
				material.DrawGui();
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

	auto model = GetActor()->GetModel().lock();
	if (model == nullptr) return;
	model->DrawGui();
}
// 指定のマテリアルのSRVを変更
void ModelRenderer::ChangeMaterialSRV(
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv,
	int materialIndex,
	std::string textureKey)
{
	_materialMap.at(materialIndex).ChangeTextureSRV(srv, textureKey);
}
// 指定のマテリアルのSRVを変更
void ModelRenderer::ChangeMaterialSRV(
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv, 
	std::string materialName, 
	std::string textureKey)
{
	for (auto& material : _materialMap)
	{
		if (material.GetName() == materialName)
		{
			material.ChangeTextureSRV(srv, textureKey);
			return;
		}
	}
}
void ModelRenderer::SetModel(std::weak_ptr<Model> model)
{
	_model = model;
	auto resource = model.lock()->GetResource();
	if (resource == nullptr) return;
	// マテリアルの取得
	_materialMap.clear();
	_materialMap = _model.lock()->GetMaterials();
}

