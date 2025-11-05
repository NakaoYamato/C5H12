#include "InstancingModelRenderer.h"

#include <imgui.h>

#include "../../Library/Scene/Scene.h"
#include "../../Library/Graphics/Graphics.h"
#include "../../Library/Shader/Model/ModelShaderResource.h"

// 生成時処理
void InstancingModelRenderer::OnCreate()
{
	_material = _model.lock()->GetMaterials().at(0);
	_material.SetShaderName("PhongAlpha");
}
// 描画処理
void InstancingModelRenderer::Render(const RenderContext& rc)
{
	if (_model.lock() == nullptr)
	{
		return;
	}

	GetActor()->GetScene()->GetMeshRenderer().DrawInstancing(
		_model.lock().get(),
		_color,
		&_material,
		GetActor()->GetTransform().GetMatrix());
}
// 影描画
void InstancingModelRenderer::CastShadow(const RenderContext& rc)
{
}
// GUI描画
void InstancingModelRenderer::DrawGui()
{
	auto modelShaderResource = ResourceManager::Instance().GetResourceAs<ModelShaderResource>();

	ImGui::ColorEdit4("color", &_color.x);
	ImGui::Separator();

	if (ImGui::TreeNode(_material.GetName().c_str()))
	{
		// シェーダー変更GUI
		modelShaderResource->DrawInstancingMaterialEditGui(&_material);
		ImGui::Separator();

		// マテリアルのGUI描画
		_material.DrawGui();
		ImGui::TreePop();
	}
	ImGui::Separator();
}
// シェーダー変更
void InstancingModelRenderer::ChangeShader(const std::string& shaderName)
{
	auto modelShaderResource = ResourceManager::Instance().GetResourceAs<ModelShaderResource>();
	_material.SetShaderName(shaderName);
	// シェーダー変更時はパラメータも初期化
	_material.SetParameterMap(modelShaderResource->GetShaderParameterKey(
		ModelRenderType::Instancing,
		shaderName,
		false));
}
