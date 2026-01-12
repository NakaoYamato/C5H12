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

    // カメラ距離計算
	auto position = GetActor()->GetTransform().GetWorldPosition();
	auto& eyePosition = GetActor()->GetScene()->GetMainCamera()->GetEye();
	_cameraDistance = Vector3::Length(position - eyePosition);

	// カメラに近ければ透明化
	if (_cameraDistanceAlphaEnabled)
	{
		_cameraDistanceAlpha = std::clamp(
			_cameraDistance / _cameraDistanceAlphaStart,
			_cameraDistanceAlphaMin,
			1.0f);
		_cameraDistanceAlpha = std::powf(_cameraDistanceAlpha, 2.0f);
	}

	if (auto model = _model.lock())
	{
		// LOD判定
		_isMidLod = false;
		_isLowLod = false;
		if (_cameraDistance > _midLodDistance && model->GetResource()->HasMiddleLODData())
		{
			if (_cameraDistance > _lowLodDistance && model->GetResource()->HasLowLODData())
			{
                _isLowLod = true;
			}
			else
			{
                _isMidLod = true;
			}
		}

		// ボーン行列計算
		const std::vector<ModelResource::Node>& nodes = model->GetPoseNodes();
		for (auto& mesh : model->GetResource()->GetMeshes())
		{
			auto& boneTransform = _boneTransformMap[mesh.nodeIndex];
			if (mesh.bones.size() > 0)
			{
				for (size_t i = 0; i < mesh.bones.size(); ++i)
				{
					const ModelResource::Bone& bone = mesh.bones.at(i);
					DirectX::XMMATRIX World = DirectX::XMLoadFloat4x4(&nodes[bone.nodeIndex].worldTransform);
					DirectX::XMMATRIX Offset = DirectX::XMLoadFloat4x4(&bone.offsetTransform);
					DirectX::XMMATRIX Bone = Offset * World;
					DirectX::XMStoreFloat4x4(&boneTransform[i], Bone);
				}
			}
			else
			{
				boneTransform[0] = nodes[mesh.nodeIndex].worldTransform;
			}
		}
	}
}
// 描画処理
void ModelRenderer::Render(const RenderContext& rc)
{
	if (_model.lock() == nullptr) return;

	Vector4 color = _color;
	color.w *= _cameraDistanceAlpha;
	const ModelResource* resource = _model.lock()->GetResource();
	const std::vector<ModelResource::Mesh>& meshes = _isMidLod ?
		resource->GetMiddleLODMeshes() :
		_isLowLod ?
		resource->GetLowLODMeshes() :
        resource->GetMeshes();
	for (const ModelResource::Mesh& mesh : meshes)
	{
		Material* material = &_materialMap.at(mesh.materialIndex);

		// 描画チェック
		if (_hiddenMeshMap[material->GetName()])
			continue;

		GetActor()->GetScene()->GetMeshRenderer().Draw(
			&mesh,
			&_boneTransformMap[mesh.nodeIndex],
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
	const std::vector<ModelResource::Mesh>& meshes = _isMidLod ?
		resource->GetMiddleLODMeshes() :
		_isLowLod ?
		resource->GetLowLODMeshes() :
		resource->GetMeshes();
	for (const ModelResource::Mesh& mesh : meshes)
	{
		Material* material = &_materialMap.at(mesh.materialIndex);

		// 描画チェック
		if (_hiddenMeshMap[material->GetName()])
			continue;

		GetActor()->GetScene()->GetMeshRenderer().DrawShadow(
			&mesh,
			&_boneTransformMap[mesh.nodeIndex],
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
    ImGui::Text(u8"カメラからの距離: %.2f m", _cameraDistance);

    ImGui::DragFloat(u8"中LODメッシュ適応距離(m)", &_midLodDistance, 0.1f, 0.0f, 1000.0f);
    ImGui::DragFloat(u8"低LODメッシュ適応距離(m)", &_lowLodDistance, 0.1f, 0.0f, 1000.0f);
    ImGui::Checkbox(u8"中LODメッシュ使用中", &_isMidLod);
    ImGui::Checkbox(u8"低LODメッシュ使用中", &_isLowLod);
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
	// ボーン行列用配列確保
	_boneTransformMap.clear();
	for (const ModelResource::Mesh& mesh : resource->GetMeshes())
	{
		_boneTransformMap[mesh.nodeIndex].resize(std::max<size_t>(mesh.bones.size(), static_cast<size_t>(1)));
	}
}

