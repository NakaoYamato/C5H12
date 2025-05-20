#include "ModelRenderer.h"

#include <imgui.h>

#include "../../Library/Scene/Scene.h"
#include "../../Library/Graphics/Graphics.h"
#include "../../Library/Algorithm/Converter.h"

// 開始処理
void ModelRenderer::Start()
{
	if (GetActor()->GetModel().lock())
	{
		// モデルが存在するならセット
		SetModel(GetActor()->GetModel());
	}
}

// 更新処理
void ModelRenderer::Update(float elapsedTime)
{
	if (_model.lock() == nullptr)
	{
		// モデルが存在しないならリターン
		auto model = GetActor()->GetModel().lock();
		if (model == nullptr) return;

		// モデルが存在するならセット
		SetModel(GetActor()->GetModel());
	}

	_model.lock()->UpdateTransform(GetActor()->GetTransform().GetMatrix());
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
	ImGui::ColorEdit4("color", &_color.x);
	ImGui::Separator();
	if (ImGui::TreeNode(u8"マテリアル"))
	{
		// 使用可能なシェーダー取得
		auto activeShaderTypes =
			GetActor()->GetScene()->GetMeshRenderer().GetShaderNames(_renderType, Graphics::Instance().RenderingDeferred());
		for (auto& material : _materialMap)
		{
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
		//SetShader("Phong");
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
	std::string filename = model.lock()->GetFilename();
	for (ModelResource::Material& modelMaterial : resource->GetAddressMaterials())
	{
		auto& material = _materialMap.emplace_back();
		material.SetName(modelMaterial.name);
		// テクスチャ情報の取得
		for (auto& [key, textureData] : modelMaterial.textureDatas)
		{

			if (textureData.filename.size() > 0)
			{
				std::filesystem::path path(filename);
				path.replace_filename(textureData.filename);
				material.LoadTexture(Graphics::Instance().GetDevice(), key, path.c_str());
			}
			else
			{
				material.MakeDummyTexture(Graphics::Instance().GetDevice(), key,
					textureData.dummyTextureValue,
					textureData.dummyTextureDimension);
			}
		}

		// カラー情報の取得
		for (auto& [key, color] : modelMaterial.colors)
		{
			material.SetColor(key, color);
		}

		// シェーダーの初期設定
		material.SetShaderName("PBR");
	}
}

