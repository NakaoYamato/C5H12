#include "ModelRenderer.h"

#include <imgui.h>

#include "../../Library/Scene/Scene.h"
#include "../../Library/Graphics/Graphics.h"
#include "../../Library/Algorithm/Converter.h"
#include "../../Library/Shader/Model/ModelShaderResource.h"

// 生成時処理
void ModelRenderer::OnCreate()
{
	if (GetActor()->GetModel().lock() && !_model.lock().get())
	{
		// モデルが存在するならセット
		SetModel(GetActor()->GetModel());
	}
}
// 遅延更新処理
void ModelRenderer::LateUpdate(float elapsedTime)
{
	// モデルが設定されていないときにアクターのモデルを検索
	if (GetActor()->GetModel().lock() && !_model.lock().get())
	{
		// モデルが存在するならセット
		SetModel(GetActor()->GetModel());
	}

	if (_cameraDistanceAlphaEnabled)
	{
		// カメラに近ければ透明化
		auto position = GetActor()->GetTransform().GetWorldPosition();
		auto& eyePosition = GetActor()->GetScene()->GetMainCamera()->GetEye();
		_cameraDistanceAlpha = std::clamp(
			Vector3::Length(position - eyePosition) / _cameraDistanceAlphaStart,
			_cameraDistanceAlphaMin,
			1.0f);
		_cameraDistanceAlpha = std::powf(_cameraDistanceAlpha, 2.0f);
	}
}
// 描画処理
void ModelRenderer::Render(const RenderContext& rc)
{
	if (_model.lock() == nullptr) return;

	Vector4 color = _color;
	color.w *= _cameraDistanceAlpha;
	const ModelResource* resource = _model.lock()->GetResource();
	for (const ModelResource::Mesh& mesh : resource->GetMeshes())
	{
		Material* material = &_materialMap.at(mesh.materialIndex);

		// 描画チェック
		if (_hiddenMeshMap[material->GetName()])
			continue;

		GetActor()->GetScene()->GetMeshRenderer().Draw(
			&mesh,
			_model.lock().get(),
			color,
			material,
			_renderType);
	}
}
// 影描画
void ModelRenderer::CastShadow(const RenderContext& rc)
{
	if (_model.lock() == nullptr) return;

	const ModelResource* resource = _model.lock()->GetResource();
	for (const ModelResource::Mesh& mesh : resource->GetMeshes())
	{
		Material* material = &_materialMap.at(mesh.materialIndex);

		// 描画チェック
		if (_hiddenMeshMap[material->GetName()])
			continue;

		GetActor()->GetScene()->GetMeshRenderer().DrawShadow(
			&mesh, 
			_model.lock().get(),
			Vector4::White,
			material,
			_renderType);
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
	auto modelShaderResource = ResourceManager::Instance().GetResourceAs<ModelShaderResource>();

	ImGui::ColorEdit4("color", &_color.x);
	ImGui::Separator();
	ImGui::Checkbox(u8"カメラ距離による透明化", &_cameraDistanceAlphaEnabled);
	if (_cameraDistanceAlphaEnabled)
	{
		ImGui::DragFloat(u8"カメラ距離によるアルファ補正値", &_cameraDistanceAlpha, 0.01f, 0.0f, 1.0f);
		ImGui::DragFloat(u8"カメラ距離によるアルファ補正最小値", &_cameraDistanceAlphaMin, 0.01f, 0.0f, 1.0f);
		ImGui::DragFloat(u8"カメラ距離によるアルファ補正開始距離(m)", &_cameraDistanceAlphaStart, 0.01f, 0.0f, 100.0f);
	}
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
			if (ImGui::TreeNode(material.GetName().c_str()))
			{
				modelShaderResource->DrawMaterialEditGui(
					&material,
					_renderType,
					Graphics::Instance().RenderingDeferred());
				ImGui::Separator();

				// マテリアルのGUI描画
				material.DrawGui();
				ImGui::TreePop();
			}
		}
		ImGui::TreePop();
	}
	ImGui::Separator();
	if (ImGui::TreeNode(u8"メッシュ非表示リスト"))
	{
		for (auto& pair : _hiddenMeshMap)
		{
			bool hidden = pair.second;
			if (ImGui::Checkbox(pair.first.c_str(), &hidden))
			{
				pair.second = hidden;
			}
		}

		ImGui::TreePop();
	}
	ImGui::Separator();

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
Material& ModelRenderer::GetMaterial(const std::string& name)
{
	for (auto& material : _materialMap)
	{
		if (material.GetName() == name)
			return material;
	}
	// 存在していない場合先頭を渡す
	return _materialMap[0];
}
void ModelRenderer::SetModel(std::weak_ptr<Model> model)
{
	_model = model;
	auto resource = model.lock()->GetResource();
	if (resource == nullptr) return;
	// マテリアルの取得
	_materialMap.clear();
	_materialMap = _model.lock()->GetMaterials();
	// パラメータの取得
	auto modelShaderResource = ResourceManager::Instance().GetResourceAs<ModelShaderResource>();
	for (auto& material : _materialMap)
	{
		bool isDeferred = Graphics::Instance().RenderingDeferred() && material.GetBlendType() != BlendType::Alpha;

		material.SetParameterMap(modelShaderResource->GetShaderParameterKey(
			_renderType,
			material.GetShaderName(),
			isDeferred));

		_hiddenMeshMap[material.GetName()] = false;
	}
}

