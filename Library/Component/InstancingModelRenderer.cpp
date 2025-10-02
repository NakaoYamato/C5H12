#include "InstancingModelRenderer.h"

#include <imgui.h>

#include "../../Library/Scene/Scene.h"
#include "../../Library/Graphics/Graphics.h"

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
	ImGui::ColorEdit4("color", &_color.x);
	ImGui::Separator();
	// 使用可能なシェーダー取得
	auto activeShaderTypes = GetActor()->GetScene()->GetMeshRenderer().GetInstancingShaderNames();
	if (ImGui::TreeNode(_material.GetName().c_str()))
	{
		// シェーダー変更GUI
		if (ImGui::TreeNode(u8"シェーダー変更"))
		{
			auto shaderType = _material.GetShaderName();
			for (auto& activeShaderType : activeShaderTypes)
			{
				bool active = activeShaderType == shaderType;
				if (ImGui::RadioButton(activeShaderType, active))
				{
					// シェーダー変更
					ChangeShader(activeShaderType);
				}
			}
			ImGui::TreePop();
		}
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
	_material.SetShaderName(shaderName);
	// シェーダー変更時はパラメータも初期化
	_material.SetParameterMap(GetActor()->GetScene()->GetMeshRenderer().GetShaderParameterKey(
		ModelRenderType::Instancing,
		shaderName,
		false));
}
