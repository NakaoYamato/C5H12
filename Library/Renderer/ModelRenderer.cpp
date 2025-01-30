#include "ModelRenderer.h"

#include "../ResourceManager/GpuResourceManager.h"
#include "../../Shader/Phong/PhongShader.h"
#include "../../Shader/Ramp/RampShader.h"

#include "../../Shader/CascadedShadowMap/CascadedShadowMapShader.h"

#include <algorithm>

static const int FBX_MAX_BONES = 256;
static const int INSTANCED_MAX = 100;

#pragma region 定数バッファ
struct DynamicBoneCB// 通常モデル用
{
	DirectX::XMFLOAT4 materialColor{ 1,1,1,1 };
	DirectX::XMFLOAT4X4 boneTransforms[FBX_MAX_BONES]
	{
		{
			1,0,0,0,
			0,1,0,0,
			0,0,1,0,
			0,0,0,1
		}
	};
};
struct StaticBoneCB// アニメーションなしモデル用
{
	DirectX::XMFLOAT4 materialColor{ 1,1,1,1 };
	DirectX::XMFLOAT4X4 world{};
};
struct InstancingModelCB// インスタンシングモデル用
{
	DirectX::XMFLOAT4 materialColor[INSTANCED_MAX]{};
	DirectX::XMFLOAT4X4 world[INSTANCED_MAX]{};
};
#pragma endregion

#pragma region Info
struct DrawInfo
{
	//ShaderBase*			shader = nullptr;
	//ModelRenderType		modelType = ModelRenderType::Dynamic;
	Model*				model = nullptr;
	const ModelResource::Mesh* mesh = nullptr;
	Vector4				color{ 1,1,1,1 };
	float				distance = 0.0f;
};
struct TransparencyDrawInfo
{
	ShaderId					shaderId = ShaderId::Phong;
	ModelRenderType				renderType = ModelRenderType::Dynamic;
	Model*						model = nullptr;
	const ModelResource::Mesh*	mesh = nullptr;
	Vector4						color{ 1,1,1,1 };
	float						distance = 0.0f;
};
// インスタンシング描画用
struct InstancingDrawInfo
{
	ShaderId				shaderId{};
	using ModelParameter = std::pair<Vector4, DirectX::XMFLOAT4X4>;
	std::vector<ModelParameter> modelParameters;
};
#pragma endregion

namespace ModelRenderer
{
	// 定数バッファのデータ
	// サイズが大きいので関数内で定義するとスタック警告がでるため静的に確保
	DynamicBoneCB		cbDynamicSkeleton_{};
	StaticBoneCB		cbStaticSkeleton_{};
	InstancingModelCB	cbInstancingSkeleton_{};

	// シェーダーの配列
	using ShaderMap = std::unordered_map<ShaderId, std::unique_ptr<ShaderBase>>;
	ShaderMap shaders_[static_cast<int>(ModelRenderType::ModelRenderTypeMax)];

	// 各モデルタイプのInfo
	using DrawInfoMap = std::unordered_map<ShaderId, std::vector<DrawInfo>>;
	using InstancingDrawInfoMap = std::unordered_map<Model*, InstancingDrawInfo>;
	DrawInfoMap                             dynamicBoneDrawInfomap_;
	DrawInfoMap                             staticBoneDrawInfomap_;
	InstancingDrawInfoMap                   instancingDrawInfoMap_;

	//std::vector<DrawInfo>					opaqueDrawInfo_;

	// 透明描画
	std::vector<TransparencyDrawInfo>		transparencyDrawInfos_;

	// 各定数バッファ
	Microsoft::WRL::ComPtr<ID3D11Buffer>	dynamicBoneCB_;
	Microsoft::WRL::ComPtr<ID3D11Buffer>	staticBoneCB_;
	Microsoft::WRL::ComPtr<ID3D11Buffer>	instancingCB_;

	namespace Helper
	{
		// インスタンシングモデルの描画
		void RenderInstancing(const RenderContext& rc)
		{
			ID3D11DeviceContext* dc = rc.deviceContext;
			// 定数バッファ設定
			dc->VSSetConstantBuffers(1, 1, instancingCB_.GetAddressOf());
			dc->PSSetConstantBuffers(1, 1, instancingCB_.GetAddressOf());

			// モデルの描画関数
			auto DrawModel = [&](Model* model, InstancingDrawInfo& drawInfo, ShaderBase* shader)->void
				{
					const ModelResource* resource = model->GetResource();
					const std::vector<ModelResource::Node>& nodes = model->GetPoseNodes();
					for (const ModelResource::Mesh& mesh : resource->GetMeshes())
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
							cbInstancingSkeleton_.materialColor[modelCount] = color;
							cbInstancingSkeleton_.world[modelCount] = world;
							modelCount++;
						}
						dc->UpdateSubresource(instancingCB_.Get(), 0, 0, &cbInstancingSkeleton_, 0, 0);

						// シェーダーの更新処理
						shader->Update(rc, &resource->GetMaterials().at(mesh.materialIndex));

						dc->DrawIndexedInstanced(static_cast<UINT>(mesh.indices.size()), static_cast<UINT>(modelCount), 0, 0, 0);
					}
				};

			// 不透明描画処理
			for (auto& drawInfomap : instancingDrawInfoMap_)
			{
				Model* model = drawInfomap.first;
				InstancingDrawInfo& drawInfo = drawInfomap.second;

				ShaderBase* shader = shaders_[static_cast<int>(ModelRenderType::Instancing)][drawInfo.shaderId].get();
				shader->Begin(rc);

				DrawModel(model, drawInfo, shader);

				shader->End(rc);
			}
			instancingDrawInfoMap_.clear();
		}

		// DynamicBoneModelのメッシュ描画
		static void DrawDynamicBoneMesh(const RenderContext& rc,
			ShaderBase* shader,
			Model* model,
			const ModelResource::Mesh* mesh,
			const Vector4& materialColor)
		{
			ID3D11DeviceContext* dc = rc.deviceContext;
			const std::vector<ModelResource::Node>& nodes = model->GetPoseNodes();

			uint32_t stride{ sizeof(ModelResource::Vertex) };
			uint32_t offset{ 0 };
			dc->IASetVertexBuffers(0, 1, mesh->vertexBuffer.GetAddressOf(), &stride, &offset);
			dc->IASetIndexBuffer(mesh->indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
			dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

			// スケルトン用定数バッファ
			if (mesh->bones.size() > 0)
			{
				for (size_t i = 0; i < mesh->bones.size(); ++i)
				{
					const ModelResource::Bone& bone = mesh->bones.at(i);
					DirectX::XMMATRIX World = DirectX::XMLoadFloat4x4(&nodes[bone.nodeIndex].worldTransform);
					DirectX::XMMATRIX Offset = DirectX::XMLoadFloat4x4(&bone.offsetTransform);
					DirectX::XMMATRIX Bone = Offset * World;
					DirectX::XMStoreFloat4x4(&cbDynamicSkeleton_.boneTransforms[i], Bone);
				}
			}
			else
			{
				cbDynamicSkeleton_.boneTransforms[0] = nodes[mesh->nodeIndex].worldTransform;
			}
			cbDynamicSkeleton_.materialColor = materialColor;

			dc->UpdateSubresource(dynamicBoneCB_.Get(), 0, 0, &cbDynamicSkeleton_, 0, 0);

			// シェーダーの更新処理
			shader->Update(rc, &model->GetResource()->GetMaterials().at(mesh->materialIndex));

			dc->DrawIndexed(static_cast<UINT>(mesh->indices.size()), 0, 0);
		}

		// StaticBoneModelのメッシュ描画
		static void DrawStaticBoneModel(const RenderContext& rc,
			ShaderBase* shader,
			Model* model,
			const ModelResource::Mesh* mesh,
			const Vector4& materialColor)
		{
			ID3D11DeviceContext* dc = rc.deviceContext;
			const ModelResource* resource = model->GetResource();
			const std::vector<ModelResource::Node>& nodes = model->GetPoseNodes();

			uint32_t stride{ sizeof(ModelResource::Vertex) };
			uint32_t offset{ 0 };
			dc->IASetVertexBuffers(0, 1, mesh->vertexBuffer.GetAddressOf(), &stride, &offset);
			dc->IASetIndexBuffer(mesh->indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
			dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

			// スケルトン用定数バッファ
			cbStaticSkeleton_.world = nodes[mesh->nodeIndex].worldTransform;
			cbStaticSkeleton_.materialColor = materialColor;
			dc->UpdateSubresource(staticBoneCB_.Get(), 0, 0, &cbStaticSkeleton_, 0, 0);

			// シェーダーの更新処理
			shader->Update(rc, &resource->GetMaterials().at(mesh->materialIndex));

			dc->DrawIndexed(static_cast<UINT>(mesh->indices.size()), 0, 0);
		}
	}

	void Initialize(ID3D11Device* device)
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
			const size_t type = static_cast<int>(ModelRenderType::Dynamic);
			ShaderMap& shaderMap = shaders_[type];

			shaderMap[ShaderId::Phong] = std::make_unique<PhongShader>(device,
				"./Data/Shader/PhongVS.cso",
				modelInputDesc, static_cast<UINT>(_countof(modelInputDesc)));

			shaderMap[ShaderId::Ramp] = std::make_unique<RampShader>(device,
				"./Data/Shader/PhongVS.cso",// フォンシェーダーと同じ処理
				modelInputDesc, static_cast<UINT>(_countof(modelInputDesc)));

			shaderMap[ShaderId::CascadedShadowMap] = std::make_unique<CascadedShadowMapShader>(device,
				"./Data/Shader/CascadedShadowVS.cso",
				modelInputDesc, static_cast<UINT>(_countof(modelInputDesc)));
		}
		{
			// StaticBoneModel
			const size_t type = static_cast<int>(ModelRenderType::Static);
			ShaderMap& shaderMap = shaders_[type];

			shaderMap[ShaderId::Phong] = std::make_unique<PhongShader>(device,
				"./Data/Shader/PhongBatchingVS.cso",
				modelInputDesc, static_cast<UINT>(_countof(modelInputDesc)));

			shaderMap[ShaderId::Ramp] = std::make_unique<RampShader>(device,
				"./Data/Shader/PhongBatchingVS.cso",// フォンシェーダーと同じ処理
				modelInputDesc, static_cast<UINT>(_countof(modelInputDesc)));

			shaderMap[ShaderId::CascadedShadowMap] = std::make_unique<CascadedShadowMapShader>(device,
				"./Data/Shader/CascadedShadowBatchingVS.cso",
				modelInputDesc, static_cast<UINT>(_countof(modelInputDesc)));
		}
		{
			// InstancingModel
			const size_t type = static_cast<int>(ModelRenderType::Instancing);
			ShaderMap& shaderMap = shaders_[type];

			shaderMap[ShaderId::Phong] = std::make_unique<PhongShader>(device,
				"./Data/Shader/PhongInstancedVS.cso",
				modelInputDesc, static_cast<UINT>(_countof(modelInputDesc)));

			shaderMap[ShaderId::Ramp] = std::make_unique<RampShader>(device,
				"./Data/Shader/PhongInstancedVS.cso",// フォンシェーダーと同じ処理
				modelInputDesc, static_cast<UINT>(_countof(modelInputDesc)));

			shaderMap[ShaderId::CascadedShadowMap] = std::make_unique<CascadedShadowMapShader>(device,
				"./Data/Shader/CascadedShadowInstancedVS.cso",
				modelInputDesc, static_cast<UINT>(_countof(modelInputDesc)));
		}
	}
	void Draw(Model* model, const Vector4& color, ShaderId shaderId, ModelRenderType renderType)
	{
		// モデルに含まれるメッシュの色を確認して不透明処理か半透明処理に振り分ける
		const ModelResource* resource = model->GetResource();
		const std::vector<ModelResource::Node>& nodes = model->GetPoseNodes();
		for (const ModelResource::Mesh& mesh : resource->GetMeshes())
		{
			// 透明度があるメッシュならTransparencyDrawInfoに登録
			if (resource->GetMaterials().at(mesh.materialIndex).colors.at("Diffuse").w < 1.0f ||
				color.w < 1.0f)
			{
				auto& transparencyDrawInfo = transparencyDrawInfos_.emplace_back();
				transparencyDrawInfo.shaderId = shaderId;
				transparencyDrawInfo.renderType = renderType;
				transparencyDrawInfo.model = model;
				transparencyDrawInfo.mesh = &mesh;
				transparencyDrawInfo.color = color;
				continue;
			}

			// 描画タイプに応じて登録
			switch (renderType)
			{
			case ModelRenderType::Dynamic:
				dynamicBoneDrawInfomap_[shaderId].push_back({ model, &mesh, color });
				break;
			case ModelRenderType::Static:
				staticBoneDrawInfomap_[shaderId].push_back({ model, &mesh, color });
				break;
			case ModelRenderType::Instancing:
				assert(!"Please Call \"DrawInstancing\"");
				break;
			default:
				assert(!"ModelRenderType Overflow");
				break;
			}
		}
	}

	void DrawInstancing(Model* model, const Vector4& color, ShaderId shaderId, const DirectX::XMFLOAT4X4& world)
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

	/// 不透明描画実行
	void RenderOpaque(const RenderContext& rc)
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
		dc->RSSetState(rc.renderState->GetRasterizerState(RasterizerState::SolidCullNone));

		// ブレンドステート設定
		dc->OMSetBlendState(rc.renderState->GetBlendState(BlendState::Opaque), nullptr, 0xFFFFFFFF);

		// ボーンの影響度があるモデルの描画
		{
			// 定数バッファ設定
			dc->VSSetConstantBuffers(1, 1, dynamicBoneCB_.GetAddressOf());
			dc->PSSetConstantBuffers(1, 1, dynamicBoneCB_.GetAddressOf());

			// 不透明描画処理
			for (auto& drawInfomap : dynamicBoneDrawInfomap_)
			{
				ShaderBase* shader = shaders_[static_cast<int>(ModelRenderType::Dynamic)][drawInfomap.first].get();
				shader->Begin(rc);

				for (auto& drawInfo : drawInfomap.second)
				{
					// メッシュ描画
					Helper::DrawDynamicBoneMesh(rc, shader, drawInfo.model, drawInfo.mesh, drawInfo.color);
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
				ShaderBase* shader = shaders_[static_cast<int>(ModelRenderType::Static)][drawInfomap.first].get();
				shader->Begin(rc);

				for (auto& drawInfo : drawInfomap.second)
				{
					// メッシュ描画
					Helper::DrawStaticBoneModel(rc, shader, drawInfo.model, drawInfo.mesh, drawInfo.color);
				}

				shader->End(rc);
			}
			staticBoneDrawInfomap_.clear();
		}

		// インスタンシングモデルの描画
		Helper::RenderInstancing(rc);
	}

	/// 半透明描画実行
	void RenderTransparency(const RenderContext& rc)
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
		dc->RSSetState(rc.renderState->GetRasterizerState(RasterizerState::SolidCullNone));

		// ブレンドステート設定
		dc->OMSetBlendState(rc.renderState->GetBlendState(BlendState::Alpha), nullptr, 0xFFFFFFFF);

		DirectX::XMVECTOR CameraPosition = DirectX::XMLoadFloat3(&rc.camera->eye_);
		DirectX::XMVECTOR CameraFront = DirectX::XMLoadFloat3(&rc.camera->front_);

		// カメラとの距離計算
		{
			for (TransparencyDrawInfo& transparencyDrawInfo : transparencyDrawInfos_)
			{
				const std::vector<ModelResource::Node>& nodes = transparencyDrawInfo.model->GetPoseNodes();
				const ModelResource::Mesh* mesh = transparencyDrawInfo.mesh;
				DirectX::XMVECTOR Position = DirectX::XMVectorSet(
					nodes[mesh->nodeIndex].worldTransform._41,
					nodes[mesh->nodeIndex].worldTransform._42,
					nodes[mesh->nodeIndex].worldTransform._43,
					0.0f);
				DirectX::XMVECTOR Vec = DirectX::XMVectorSubtract(Position, CameraPosition);
				transparencyDrawInfo.distance = DirectX::XMVectorGetX(DirectX::XMVector3Dot(CameraFront, Vec));
			}
		}

		// カメラから遠い順にソート
		std::sort(transparencyDrawInfos_.begin(), transparencyDrawInfos_.end(),
			[](const TransparencyDrawInfo& lhs, const TransparencyDrawInfo& rhs)
			{
				return lhs.distance > rhs.distance;
			});

		// 描画
		for (const TransparencyDrawInfo& transparencyDrawInfo : transparencyDrawInfos_)
		{
			ShaderBase* shader = shaders_[static_cast<int>(transparencyDrawInfo.renderType)][transparencyDrawInfo.shaderId].get();

			shader->Begin(rc);

			switch (transparencyDrawInfo.renderType)
			{
			case ModelRenderType::Dynamic:
				// 定数バッファ設定
				dc->VSSetConstantBuffers(1, 1, dynamicBoneCB_.GetAddressOf());
				dc->PSSetConstantBuffers(1, 1, dynamicBoneCB_.GetAddressOf());
				Helper::DrawDynamicBoneMesh(rc, shader, transparencyDrawInfo.model, transparencyDrawInfo.mesh, transparencyDrawInfo.color);
				break;
			case ModelRenderType::Static:
				dc->VSSetConstantBuffers(1, 1, staticBoneCB_.GetAddressOf());
				dc->PSSetConstantBuffers(1, 1, staticBoneCB_.GetAddressOf());
				Helper::DrawStaticBoneModel(rc, shader, transparencyDrawInfo.model, transparencyDrawInfo.mesh, transparencyDrawInfo.color);
				break;
			case ModelRenderType::Instancing:

				break;
			}

			shader->End(rc);
		}
		transparencyDrawInfos_.clear();
	}

	void CastShadow(const RenderContext& rc)
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
				ShaderBase* shader = shaders_[static_cast<int>(ModelRenderType::Dynamic)][drawInfomap.first].get();
				shader->Begin(rc);

				for (auto& drawInfo : drawInfomap.second)
				{
					// メッシュ描画
					auto model = drawInfo.model;
					auto& color = drawInfo.color;
					const ModelResource* resource = model->GetResource();
					const std::vector<ModelResource::Node>& nodes = model->GetPoseNodes();
					for (const ModelResource::Mesh& mesh : resource->GetMeshes())
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
								DirectX::XMStoreFloat4x4(&cbDynamicSkeleton_.boneTransforms[i], Bone);
							}
						}
						else
						{
							cbDynamicSkeleton_.boneTransforms[0] = nodes[mesh.nodeIndex].worldTransform;
						}
						cbDynamicSkeleton_.materialColor = color;

						dc->UpdateSubresource(dynamicBoneCB_.Get(), 0, 0, &cbDynamicSkeleton_, 0, 0);

						// シェーダーの更新処理
						shader->Update(rc, &resource->GetMaterials().at(mesh.materialIndex));

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
				ShaderBase* shader = shaders_[static_cast<int>(ModelRenderType::Static)][drawInfomap.first].get();
				shader->Begin(rc);

				for (auto& drawInfo : drawInfomap.second)
				{
					// メッシュ描画
					auto model = drawInfo.model;
					auto& color = drawInfo.color;
					const ModelResource* resource = model->GetResource();
					const std::vector<ModelResource::Node>& nodes = model->GetPoseNodes();
					for (const ModelResource::Mesh& mesh : resource->GetMeshes())
					{
						uint32_t stride{ sizeof(ModelResource::Vertex) };
						uint32_t offset{ 0 };
						dc->IASetVertexBuffers(0, 1, mesh.vertexBuffer.GetAddressOf(), &stride, &offset);
						dc->IASetIndexBuffer(mesh.indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
						dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

						// スケルトン用定数バッファ
						cbStaticSkeleton_.world = nodes[mesh.nodeIndex].worldTransform;
						cbStaticSkeleton_.materialColor = color;
						dc->UpdateSubresource(staticBoneCB_.Get(), 0, 0, &cbStaticSkeleton_, 0, 0);

						// シェーダーの更新処理
						shader->Update(rc, &resource->GetMaterials().at(mesh.materialIndex));

						dc->DrawIndexedInstanced(static_cast<UINT>(mesh.indices.size()), CASCADED_SHADOW_MAPS_SIZE, 0, 0, 0);
					}
				}

				shader->End(rc);
			}
			staticBoneDrawInfomap_.clear();
		}

		// インスタンシングモデルの描画
		Helper::RenderInstancing(rc);
	}
}