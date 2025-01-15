#include "ModelRenderer.h"

#include "../ResourceManager/GpuResourceManager.h"
#include "../../Shader/Basic/BasicShader.h"
#include "../../Shader/Phong/PhongShader.h"
#include "../../Shader/CascadedShadowMap/CascadedShadowMapShader.h"

#include <algorithm>

ModelRenderer::ModelRenderer(ID3D11Device* device)
{
	// 通常モデル用定数バッファ
	GpuResourceManager::CreateConstantBuffer(
		device,
		sizeof(DynamicBoneCB),
		dynamicBoneCB_.GetAddressOf());
	// アニメーションなしモデル用定数バッファ
	GpuResourceManager::CreateConstantBuffer(
		device,
		sizeof(StaticBoneCB),
		staticBoneCB_.GetAddressOf());
	// インスタンシングモデル用定数バッファ
	GpuResourceManager::CreateConstantBuffer(
		device,
		sizeof(InstancingModelCB),
		instancingCB_.GetAddressOf());

	// インプットレイアウト
	D3D11_INPUT_ELEMENT_DESC modelInputDesc[]
	{
		{"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0, D3D11_APPEND_ALIGNED_ELEMENT},
		{"WEIGHTS",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT},
		{"BONES",0,DXGI_FORMAT_R32G32B32A32_UINT,0,D3D11_APPEND_ALIGNED_ELEMENT},
		{"TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT},
		{"NORMAL",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT},
		{"TANGENT",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT},
	};

	// シェーダー作成
	{
		// DynamicBoneModel
		const size_t type = static_cast<int>(ModelRenderType::DynamicBoneModel);
		ShaderMap& shaderMap = shaders_[type];

		shaderMap[ShaderId::Basic] = std::make_unique<BasicShader>(device,
			"./Data/Shader/BasicVS.cso",
			modelInputDesc, static_cast<UINT>(_countof(modelInputDesc)));
		shaderMap[ShaderId::Phong] = std::make_unique<PhongShader>(device,
			"./Data/Shader/PhongVS.cso",
			modelInputDesc, static_cast<UINT>(_countof(modelInputDesc)));
		shaderMap[ShaderId::CascadedShadowMap] = std::make_unique<CascadedShadowMapShader>(device,
			"./Data/Shader/CascadedShadowVS.cso",
			modelInputDesc, static_cast<UINT>(_countof(modelInputDesc)));
	}
	{
		// StaticBoneModel
		const size_t type = static_cast<int>(ModelRenderType::StaticBoneModel);
		ShaderMap& shaderMap = shaders_[type];

		shaderMap[ShaderId::Basic] = std::make_unique<BasicShader>(device,
			"./Data/Shader/BasicBatchingVS.cso",
			modelInputDesc, static_cast<UINT>(_countof(modelInputDesc)));
		shaderMap[ShaderId::Phong] = std::make_unique<PhongShader>(device,
			"./Data/Shader/PhongBatchingVS.cso",
			modelInputDesc, static_cast<UINT>(_countof(modelInputDesc)));
		shaderMap[ShaderId::CascadedShadowMap] = std::make_unique<CascadedShadowMapShader>(device,
			"./Data/Shader/CascadedShadowBatchingVS.cso",
			modelInputDesc, static_cast<UINT>(_countof(modelInputDesc)));
	}
	{
		// InstancingModel
		const size_t type = static_cast<int>(ModelRenderType::InstancingModel);
		ShaderMap& shaderMap = shaders_[type];

		shaderMap[ShaderId::Basic] = std::make_unique<BasicShader>(device,
			"./Data/Shader/BasicInstancedVS.cso",
			modelInputDesc, static_cast<UINT>(_countof(modelInputDesc)));
		shaderMap[ShaderId::Phong] = std::make_unique<PhongShader>(device,
			"./Data/Shader/PhongInstancedVS.cso",
			modelInputDesc, static_cast<UINT>(_countof(modelInputDesc)));
		shaderMap[ShaderId::CascadedShadowMap] = std::make_unique<CascadedShadowMapShader>(device,
			"./Data/Shader/CascadedShadowInstancedVS.cso",
			modelInputDesc, static_cast<UINT>(_countof(modelInputDesc)));
	}
}

/// モデルの描画
void ModelRenderer::Draw(Model* model,
	const Vector4& color, 
	ShaderId shaderId, 
	ModelRenderType renderType)
{
	// 描画タイプに応じて登録
	switch (renderType)
	{
	case ModelRenderType::DynamicBoneModel:
		dynamicBoneDrawInfomap_[shaderId].push_back({ model,color });
		break;
	case ModelRenderType::StaticBoneModel:
		staticBoneDrawInfomap_[shaderId].push_back({ model,color });
		break;
	case ModelRenderType::InstancingModel:
		assert(!"Please Call \"DrawInstancing\"");
		break;
	default:
		assert(!"ModelRenderType Overflow");
		break;
	}
}

/// インスタンシングモデルの描画
void ModelRenderer::DrawInstancing(Model* model, 
	const Vector4& color,
	ShaderId shaderId,
	const DirectX::XMFLOAT4X4& world)
{
	// 過去に登録しているか確認
	auto iter = instancingDrawInfoMap_.find(model);

	if (iter != instancingDrawInfoMap_.end())
	{
		// 既に登録している場合はパラメータを追加
		(*iter).second.modelParameters.push_back(std::make_pair(color, world));
	}
	else
	{
		// ない場合は新規で登録
		InstancingDrawInfo newInfo;
		newInfo.shaderId = shaderId;
		newInfo.modelParameters.push_back(std::make_pair(color, world));
		instancingDrawInfoMap_[model] = newInfo;
	}
}

/// 描画実行
void ModelRenderer::Render(const RenderContext& rc)
{
	ID3D11DeviceContext* dc = rc.deviceContext;

	// シーン用定数バッファはこの関数を呼ぶ前に行う

	// サンプラステート設定
	ID3D11SamplerState* samplerStates[] =
	{
		rc.renderState->GetSamplerState(SamplerState::LinearWrap)
	};
	dc->PSSetSamplers(0, _countof(samplerStates), samplerStates);

	// レンダーステート設定
	dc->OMSetDepthStencilState(rc.renderState->GetDepthStencilState(DepthState::TestAndWrite), 0);
	dc->RSSetState(rc.renderState->GetRasterizerState(RasterizerState::SolidCullNone));

	// ブレンドステート設定
	dc->OMSetBlendState(rc.renderState->GetBlendState(BlendState::Alpha), nullptr, 0xFFFFFFFF);

	DirectX::XMVECTOR CameraPosition = DirectX::XMLoadFloat3(&rc.camera->eye_);
	DirectX::XMVECTOR CameraFront = DirectX::XMLoadFloat3(&rc.camera->front_);

	// ボーンの影響度があるモデルの描画
	{
		// 定数バッファ設定
		dc->VSSetConstantBuffers(1, 1, dynamicBoneCB_.GetAddressOf());
		dc->PSSetConstantBuffers(1, 1, dynamicBoneCB_.GetAddressOf());

		// 不透明描画処理
		for (auto& drawInfomap : dynamicBoneDrawInfomap_)
		{
			ShaderBase* shader = shaders_[static_cast<int>(ModelRenderType::DynamicBoneModel)][drawInfomap.first].get();
			shader->Begin(rc);

			for (auto& drawInfo : drawInfomap.second)
			{
				// メッシュ描画
				DrawDynamicBoneMesh(rc, CameraPosition, CameraFront, drawInfo.model, drawInfo.color, shader);
			}

			shader->End(rc);
		}
		dynamicBoneDrawInfomap_.clear();
	}

	// ボーンの影響度がないモデルの描画
	{
		ID3D11DeviceContext* dc = rc.deviceContext;
		// 定数バッファ設定
		dc->VSSetConstantBuffers(1, 1, staticBoneCB_.GetAddressOf());
		dc->PSSetConstantBuffers(1, 1, staticBoneCB_.GetAddressOf());

		// 不透明描画処理
		for (auto& drawInfomap : staticBoneDrawInfomap_)
		{
			ShaderBase* shader = shaders_[static_cast<int>(ModelRenderType::StaticBoneModel)][drawInfomap.first].get();
			shader->Begin(rc);

			for (auto& drawInfo : drawInfomap.second)
			{
				// メッシュ描画
				DrawStaticBoneModel(rc, CameraPosition, CameraFront, drawInfo.model, drawInfo.color, shader);
			}

			shader->End(rc);
		}
		staticBoneDrawInfomap_.clear();
	}

	// インスタンシングモデルの描画
	RenderInstancing(rc);

	// ブレンドステート設定
	dc->OMSetBlendState(rc.renderState->GetBlendState(BlendState::Transparency), nullptr, 0xFFFFFFFF);

	// カメラから遠い順にソート
	std::sort(dynamicTransparencies_.begin(), dynamicTransparencies_.end(),
		[](const TransparencyDrawInfo& lhs, const TransparencyDrawInfo& rhs)
		{
			return lhs.distance > rhs.distance;
		});
	std::sort(staticTransparencies_.begin(), staticTransparencies_.end(),
		[](const TransparencyDrawInfo& lhs, const TransparencyDrawInfo& rhs)
		{
			return lhs.distance > rhs.distance;
		});

	// 半透明描画処理
	{
		// ボーンの影響度があるモデルの描画
		{
			dc->VSSetConstantBuffers(1, 1, dynamicBoneCB_.GetAddressOf());
			dc->PSSetConstantBuffers(1, 1, dynamicBoneCB_.GetAddressOf());
			for (const TransparencyDrawInfo& transparencyDrawInfo : dynamicTransparencies_)
			{
				ShaderBase* shader = transparencyDrawInfo.shader;

				shader->Begin(rc);

				const std::vector<ModelResource::Node>& nodes = transparencyDrawInfo.model->GetNodes();
				const ModelResource::Mesh& mesh = *transparencyDrawInfo.mesh;
				uint32_t stride{ sizeof(ModelResource::Vertex) };
				uint32_t offset{ 0 };
				dc->IASetVertexBuffers(0, 1, mesh.vertexBuffer.GetAddressOf(), &stride, &offset);
				dc->IASetIndexBuffer(mesh.indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
				dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

				// スケルトン用定数バッファ
				if (mesh.bones.size() > 0)
				{
					for (size_t i = 0; i < mesh.bones.size(); ++i)
					{
						const ModelResource::Bone& bone = mesh.bones.at(i);
						DirectX::XMMATRIX World = DirectX::XMLoadFloat4x4(&nodes[bone.nodeIndex].worldTransform);
						DirectX::XMMATRIX Offset = DirectX::XMLoadFloat4x4(&bone.offsetTransform);
						DirectX::XMMATRIX Bone = Offset * World;
						DirectX::XMStoreFloat4x4(&cbSkeleton_.boneTransforms[i], Bone);
					}
				}
				else
				{
					cbSkeleton_.boneTransforms[0] = nodes[mesh.nodeIndex].worldTransform;
				}
				cbSkeleton_.materialColor = transparencyDrawInfo.color;

				dc->UpdateSubresource(dynamicBoneCB_.Get(), 0, 0, &cbSkeleton_, 0, 0);

				// シェーダーの更新処理
				shader->Update(rc, &transparencyDrawInfo.model->GetMaterials().at(mesh.materialIndex));

				dc->DrawIndexed(static_cast<UINT>(mesh.indices.size()), 0, 0);

				shader->End(rc);
			}
			dynamicTransparencies_.clear();
		}

		// ボーンの影響度がないモデルの描画
		{
			dc->VSSetConstantBuffers(1, 1, staticBoneCB_.GetAddressOf());
			dc->PSSetConstantBuffers(1, 1, staticBoneCB_.GetAddressOf());
			for (const TransparencyDrawInfo& transparencyDrawInfo : staticTransparencies_)
			{
				ShaderBase* shader = transparencyDrawInfo.shader;

				shader->Begin(rc);

				const std::vector<ModelResource::Node>& nodes = transparencyDrawInfo.model->GetNodes();
				const ModelResource::Mesh& mesh = *transparencyDrawInfo.mesh;
				uint32_t stride{ sizeof(ModelResource::Vertex) };
				uint32_t offset{ 0 };
				dc->IASetVertexBuffers(0, 1, mesh.vertexBuffer.GetAddressOf(), &stride, &offset);
				dc->IASetIndexBuffer(mesh.indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
				dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

				// スケルトン用定数バッファ
				cbBSkeleton_.world = nodes[mesh.nodeIndex].worldTransform;
				cbBSkeleton_.materialColor = transparencyDrawInfo.color;
				dc->UpdateSubresource(staticBoneCB_.Get(), 0, 0, &cbBSkeleton_, 0, 0);

				// シェーダーの更新処理
				shader->Update(rc, &transparencyDrawInfo.model->GetMaterials().at(mesh.materialIndex));

				dc->DrawIndexed(static_cast<UINT>(mesh.indices.size()), 0, 0);

				shader->End(rc);
			}
			staticTransparencies_.clear();
		}
	}
}

/// 影描画実行
void ModelRenderer::CastShadow(const RenderContext& rc)
{
	ID3D11DeviceContext* dc = rc.deviceContext;

	// サンプラステート設定
	ID3D11SamplerState* samplerStates[] =
	{
		rc.renderState->GetSamplerState(SamplerState::LinearWrap)
	};
	dc->PSSetSamplers(0, _countof(samplerStates), samplerStates);

	// レンダーステート設定
	dc->OMSetDepthStencilState(rc.renderState->GetDepthStencilState(DepthState::TestAndWrite), 0);
	dc->RSSetState(rc.renderState->GetRasterizerState(RasterizerState::SolidCullBack));
	// ブレンドステート設定
	dc->OMSetBlendState(rc.renderState->GetBlendState(BlendState::Transparency), nullptr, 0xFFFFFFFF);

	// ボーンの影響度があるモデルの描画
	{
		// 定数バッファ設定
		dc->VSSetConstantBuffers(1, 1, dynamicBoneCB_.GetAddressOf());
		dc->PSSetConstantBuffers(1, 1, dynamicBoneCB_.GetAddressOf());

		// 不透明描画処理
		for (auto& drawInfomap : dynamicBoneDrawInfomap_)
		{
			ShaderBase* shader = shaders_[static_cast<int>(ModelRenderType::DynamicBoneModel)][drawInfomap.first].get();
			shader->Begin(rc);

			for (auto& drawInfo : drawInfomap.second)
			{
				// メッシュ描画
				auto model = drawInfo.model;
				auto& color = drawInfo.color;
				const std::vector<ModelResource::Node>& nodes = model->GetNodes();
				for (const ModelResource::Mesh& mesh : model->GetMesh())
				{
					uint32_t stride{ sizeof(ModelResource::Vertex) };
					uint32_t offset{ 0 };
					dc->IASetVertexBuffers(0, 1, mesh.vertexBuffer.GetAddressOf(), &stride, &offset);
					dc->IASetIndexBuffer(mesh.indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
					dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

					// スケルトン用定数バッファ
					if (mesh.bones.size() > 0)
					{
						for (size_t i = 0; i < mesh.bones.size(); ++i)
						{
							const ModelResource::Bone& bone = mesh.bones.at(i);
							DirectX::XMMATRIX World = DirectX::XMLoadFloat4x4(&nodes[bone.nodeIndex].worldTransform);
							DirectX::XMMATRIX Offset = DirectX::XMLoadFloat4x4(&bone.offsetTransform);
							DirectX::XMMATRIX Bone = Offset * World;
							DirectX::XMStoreFloat4x4(&cbSkeleton_.boneTransforms[i], Bone);
						}
					}
					else
					{
						cbSkeleton_.boneTransforms[0] = nodes[mesh.nodeIndex].worldTransform;
					}
					cbSkeleton_.materialColor = color;

					dc->UpdateSubresource(dynamicBoneCB_.Get(), 0, 0, &cbSkeleton_, 0, 0);

					// シェーダーの更新処理
					shader->Update(rc, &model->GetMaterials().at(mesh.materialIndex));

					dc->DrawIndexedInstanced(static_cast<UINT>(mesh.indices.size()), CASCADED_SHADOW_MAPS_SIZE, 0, 0, 0);
				}
			}

			shader->End(rc);
		}
		dynamicBoneDrawInfomap_.clear();
	}

	// ボーンの影響度がないモデルの描画
	{
		ID3D11DeviceContext* dc = rc.deviceContext;
		// 定数バッファ設定
		dc->VSSetConstantBuffers(1, 1, staticBoneCB_.GetAddressOf());
		dc->PSSetConstantBuffers(1, 1, staticBoneCB_.GetAddressOf());

		// 不透明描画処理
		for (auto& drawInfomap : staticBoneDrawInfomap_)
		{
			ShaderBase* shader = shaders_[static_cast<int>(ModelRenderType::StaticBoneModel)][drawInfomap.first].get();
			shader->Begin(rc);

			for (auto& drawInfo : drawInfomap.second)
			{
				// メッシュ描画
				auto model = drawInfo.model;
				auto& color = drawInfo.color;
				const std::vector<ModelResource::Node>& nodes = model->GetNodes();
				for (const ModelResource::Mesh& mesh : model->GetMesh())
				{
					uint32_t stride{ sizeof(ModelResource::Vertex) };
					uint32_t offset{ 0 };
					dc->IASetVertexBuffers(0, 1, mesh.vertexBuffer.GetAddressOf(), &stride, &offset);
					dc->IASetIndexBuffer(mesh.indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
					dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

					// スケルトン用定数バッファ
					cbBSkeleton_.world = nodes[mesh.nodeIndex].worldTransform;
					cbBSkeleton_.materialColor = color;
					dc->UpdateSubresource(staticBoneCB_.Get(), 0, 0, &cbBSkeleton_, 0, 0);

					// シェーダーの更新処理
					shader->Update(rc, &model->GetMaterials().at(mesh.materialIndex));

					dc->DrawIndexedInstanced(static_cast<UINT>(mesh.indices.size()), CASCADED_SHADOW_MAPS_SIZE, 0, 0, 0);
				}
			}

			shader->End(rc);
		}
		staticBoneDrawInfomap_.clear();
	}

	// インスタンシングモデルの描画
	RenderInstancing(rc);
}

// インスタンシングモデルの描画
void ModelRenderer::RenderInstancing(const RenderContext& rc)
{
	ID3D11DeviceContext* dc = rc.deviceContext;
	// 定数バッファ設定
	dc->VSSetConstantBuffers(1, 1, instancingCB_.GetAddressOf());
	dc->PSSetConstantBuffers(1, 1, instancingCB_.GetAddressOf());

	// モデルの描画関数
	auto DrawModel = [&](Model* model, InstancingDrawInfo& drawInfo, ShaderBase* shader)->void
		{
			const std::vector<ModelResource::Node>& nodes = model->GetNodes();
			for (const ModelResource::Mesh& mesh : model->GetMesh())
			{
				// TODO : 透明度処理

				uint32_t stride{ sizeof(ModelResource::Vertex) };
				uint32_t offset{ 0 };
				dc->IASetVertexBuffers(0, 1, mesh.vertexBuffer.GetAddressOf(), &stride, &offset);
				dc->IASetIndexBuffer(mesh.indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
				dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

				// 定数バッファの設定
				// TODO : INSTANCED_MAX以上のモデル描画
				assert(drawInfo.modelParameters.size() < INSTANCED_MAX);
				size_t modelCount = 0;
				for (auto& [color, world] : drawInfo.modelParameters)
				{
					cbISkeleton_.materialColor[modelCount] = color;
					cbISkeleton_.world[modelCount] = world;
					modelCount++;
				}
				dc->UpdateSubresource(instancingCB_.Get(), 0, 0, &cbISkeleton_, 0, 0);

				// シェーダーの更新処理
				shader->Update(rc, &model->GetMaterials().at(mesh.materialIndex));

				dc->DrawIndexedInstanced(static_cast<UINT>(mesh.indices.size()), static_cast<UINT>(modelCount), 0, 0, 0);
			}
		};

	// 不透明描画処理
	for (auto& drawInfomap : instancingDrawInfoMap_)
	{
		Model* model = drawInfomap.first;
		InstancingDrawInfo& drawInfo = drawInfomap.second;

		ShaderBase* shader = shaders_[static_cast<int>(ModelRenderType::InstancingModel)][drawInfo.shaderId].get();
		shader->Begin(rc);

		DrawModel(model, drawInfo, shader);

		shader->End(rc);
	}
	instancingDrawInfoMap_.clear();
}

// DynamicBoneModelのメッシュ描画
void ModelRenderer::DrawDynamicBoneMesh(const RenderContext& rc, 
	const DirectX::XMVECTOR& CameraPosition,
	const DirectX::XMVECTOR& CameraFront,
	Model* model,
	const Vector4& materialColor,
	ShaderBase* shader)
{
	ID3D11DeviceContext* dc = rc.deviceContext;
	const std::vector<ModelResource::Node>& nodes = model->GetNodes();
	for (const ModelResource::Mesh& mesh : model->GetMesh())
	{
		// 透明度があるメッシュならTransparencyDrawInfoに登録
		if (model->GetMaterials().at(mesh.materialIndex).colors.at("Diffuse").w < 1.0f ||
			materialColor.w < 1.0f)
		{
			TransparencyDrawInfo& transparencyDrawInfo = dynamicTransparencies_.emplace_back();
			transparencyDrawInfo.shader = shader;
			transparencyDrawInfo.model = model;
			transparencyDrawInfo.mesh = &mesh;
			transparencyDrawInfo.color = materialColor;
			// カメラとの距離を算出
			DirectX::XMVECTOR Position = DirectX::XMVectorSet(
				nodes[mesh.nodeIndex].worldTransform._41,
				nodes[mesh.nodeIndex].worldTransform._42,
				nodes[mesh.nodeIndex].worldTransform._43,
				0.0f);
			DirectX::XMVECTOR Vec = DirectX::XMVectorSubtract(Position, CameraPosition);
			transparencyDrawInfo.distance = DirectX::XMVectorGetX(DirectX::XMVector3Dot(CameraFront, Vec));

			continue;
		}

		uint32_t stride{ sizeof(ModelResource::Vertex) };
		uint32_t offset{ 0 };
		dc->IASetVertexBuffers(0, 1, mesh.vertexBuffer.GetAddressOf(), &stride, &offset);
		dc->IASetIndexBuffer(mesh.indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
		dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// スケルトン用定数バッファ
		if (mesh.bones.size() > 0)
		{
			for (size_t i = 0; i < mesh.bones.size(); ++i)
			{
				const ModelResource::Bone& bone = mesh.bones.at(i);
				DirectX::XMMATRIX World = DirectX::XMLoadFloat4x4(&nodes[bone.nodeIndex].worldTransform);
				DirectX::XMMATRIX Offset = DirectX::XMLoadFloat4x4(&bone.offsetTransform);
				DirectX::XMMATRIX Bone = Offset * World;
				DirectX::XMStoreFloat4x4(&cbSkeleton_.boneTransforms[i], Bone);
			}
		}
		else
		{
			cbSkeleton_.boneTransforms[0] = nodes[mesh.nodeIndex].worldTransform;
		}
		cbSkeleton_.materialColor = materialColor;

		dc->UpdateSubresource(dynamicBoneCB_.Get(), 0, 0, &cbSkeleton_, 0, 0);

		// シェーダーの更新処理
		shader->Update(rc, &model->GetMaterials().at(mesh.materialIndex));

		dc->DrawIndexed(static_cast<UINT>(mesh.indices.size()), 0, 0);
	}
}

// StaticBoneModelのメッシュ描画
void ModelRenderer::DrawStaticBoneModel(const RenderContext& rc, 
	const DirectX::XMVECTOR& CameraPosition,
	const DirectX::XMVECTOR& CameraFront, 
	Model* model, 
	const Vector4& materialColor, 
	ShaderBase* shader)
{
	ID3D11DeviceContext* dc = rc.deviceContext;
	const std::vector<ModelResource::Node>& nodes = model->GetNodes();
	for (const ModelResource::Mesh& mesh : model->GetMesh())
	{
		// 透明度があるメッシュならTransparencyDrawInfoに登録
		if (model->GetMaterials().at(mesh.materialIndex).colors.at("Diffuse").w < 1.0f ||
			materialColor.w < 1.0f)
		{
			TransparencyDrawInfo& transparencyDrawInfo = staticTransparencies_.emplace_back();
			transparencyDrawInfo.shader = shader;
			transparencyDrawInfo.model = model;
			transparencyDrawInfo.mesh = &mesh;
			transparencyDrawInfo.color = materialColor;
			// カメラとの距離を算出
			DirectX::XMVECTOR Position = DirectX::XMVectorSet(
				nodes[mesh.nodeIndex].worldTransform._41,
				nodes[mesh.nodeIndex].worldTransform._42,
				nodes[mesh.nodeIndex].worldTransform._43,
				0.0f);
			DirectX::XMVECTOR Vec = DirectX::XMVectorSubtract(Position, CameraPosition);
			transparencyDrawInfo.distance = DirectX::XMVectorGetX(DirectX::XMVector3Dot(CameraFront, Vec));

			continue;
		}

		uint32_t stride{ sizeof(ModelResource::Vertex) };
		uint32_t offset{ 0 };
		dc->IASetVertexBuffers(0, 1, mesh.vertexBuffer.GetAddressOf(), &stride, &offset);
		dc->IASetIndexBuffer(mesh.indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
		dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// スケルトン用定数バッファ
		cbBSkeleton_.world = nodes[mesh.nodeIndex].worldTransform;
		cbBSkeleton_.materialColor = materialColor;
		dc->UpdateSubresource(staticBoneCB_.Get(), 0, 0, &cbBSkeleton_, 0, 0);

		// シェーダーの更新処理
		shader->Update(rc, &model->GetMaterials().at(mesh.materialIndex));

		dc->DrawIndexed(static_cast<UINT>(mesh.indices.size()), 0, 0);
	}
}
