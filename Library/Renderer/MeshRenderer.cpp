#include "MeshRenderer.h"

#include "../ResourceManager/GpuResourceManager.h"
#include "../../Shader/Phong/PhongShader.h"
#include "../../Shader/Ramp/RampShader.h"
#include "../../Shader/Grass/GrassShader.h"
#include "../../Shader/PBR/PBRShader.h"

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
	Model*				model = nullptr;
	const ModelResource::Mesh* mesh = nullptr;
	Vector4				color{ 1,1,1,1 };
	ShaderBase::Parameter* parameter;
};
struct AlphaDrawInfo
{
	Model* model = nullptr;
	const ModelResource::Mesh* mesh = nullptr;
	Vector4				color{ 1,1,1,1 };
	ShaderBase::Parameter* parameter;
	std::string shaderID;
	ModelRenderType renderType;
	float			distance = 0.0f;
};
// インスタンシング描画用
struct InstancingDrawInfo
{
	std::string				shaderId{};
	using ModelParameter = std::tuple<Vector4, DirectX::XMFLOAT4X4>;
	std::vector<ModelParameter> modelParameters;
	ShaderBase::Parameter* parameter = nullptr;
};
#pragma endregion

namespace MeshRenderer
{
	// 定数バッファのデータ
	// サイズが大きいので関数内で定義するとスタック警告がでるため静的に確保
	DynamicBoneCB		cbDynamicSkeleton_{};
	StaticBoneCB		cbStaticSkeleton_{};
	InstancingModelCB	cbInstancingSkeleton_{};

	// シェーダーの配列
	using ShaderMap = std::unordered_map<std::string, std::unique_ptr<ShaderBase>>;
	ShaderMap deferredShaders_[static_cast<int>(ModelRenderType::ModelRenderTypeMax)];
	ShaderMap forwardShaders_[static_cast<int>(ModelRenderType::ModelRenderTypeMax)];
	std::unique_ptr<ShaderBase> cascadedSMShader_[static_cast<int>(ModelRenderType::ModelRenderTypeMax)];

	// 各モデルタイプのInfo
	using DrawInfoMap = std::unordered_map<std::string, std::vector<DrawInfo>>;
	using InstancingDrawInfoMap = std::unordered_map<Model*, InstancingDrawInfo>;
	DrawInfoMap                             dynamicInfomap_;
	DrawInfoMap                             staticInfomap_;
	//DrawInfoMap                             dynamicTransparencyInfomap_;
	//DrawInfoMap                             staticTransparencyInfomap_;
	std::vector<AlphaDrawInfo> alphaDrawInfomap_;
	InstancingDrawInfoMap                   instancingInfoMap_;

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
						shader->Update(rc, &resource->GetMaterials().at(mesh.materialIndex), drawInfo.parameter);

						dc->DrawIndexedInstanced(static_cast<UINT>(mesh.indices.size()), static_cast<UINT>(modelCount), 0, 0, 0);
					}
				};

			// 不透明描画処理
			for (auto& drawInfomap : instancingInfoMap_)
			{
				Model* model = drawInfomap.first;
				InstancingDrawInfo& drawInfo = drawInfomap.second;

				ShaderBase* shader = deferredShaders_[static_cast<int>(ModelRenderType::Instancing)][drawInfo.shaderId].get();
				shader->Begin(rc);

				DrawModel(model, drawInfo, shader);

				shader->End(rc);
			}
			instancingInfoMap_.clear();
		}

		// DynamicBoneModelのメッシュ描画
		static void DrawDynamicBoneMesh(const RenderContext& rc,
			ShaderBase* shader,
			Model* model,
			const ModelResource::Mesh* mesh,
			const Vector4& materialColor,
			ShaderBase::Parameter* parameter)
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
			shader->Update(rc, &model->GetResource()->GetMaterials().at(mesh->materialIndex), parameter);

			dc->DrawIndexed(static_cast<UINT>(mesh->indices.size()), 0, 0);
		}

		// StaticBoneModelのメッシュ描画
		static void DrawStaticBoneModel(const RenderContext& rc,
			ShaderBase* shader,
			Model* model,
			const ModelResource::Mesh* mesh,
			const Vector4& materialColor,
			ShaderBase::Parameter* parameter)
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
			shader->Update(rc, &resource->GetMaterials().at(mesh->materialIndex), parameter);

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
			// デファードレンダリング用
			{
				{
					// DynamicBoneModel
					const size_t type = static_cast<int>(ModelRenderType::Dynamic);
					ShaderMap& shaderMap = deferredShaders_[type];

					shaderMap["Phong"] = std::make_unique<PhongShader>(device,
						"./Data/Shader/PhongVS.cso",
						"./Data/Shader/PhongGBPS.cso",
						modelInputDesc, static_cast<UINT>(_countof(modelInputDesc)));

					shaderMap["Ramp"] = std::make_unique<RampShader>(device,
						"./Data/Shader/PhongVS.cso",// フォンシェーダーと同じ処理
						modelInputDesc, static_cast<UINT>(_countof(modelInputDesc)));

					shaderMap["PBR"] = std::make_unique<PBRShader>(device,
						"./Data/Shader/PhongVS.cso",
						"./Data/Shader/PhysicalBasedRenderingGBPS.cso",
						modelInputDesc, static_cast<UINT>(_countof(modelInputDesc)));
				}
				{
					// StaticBoneModel
					const size_t type = static_cast<int>(ModelRenderType::Static);
					ShaderMap& shaderMap = deferredShaders_[type];

					shaderMap["Phong"] = std::make_unique<PhongShader>(device,
						"./Data/Shader/PhongBatchingVS.cso",
						"./Data/Shader/PhongGBPS.cso",
						modelInputDesc, static_cast<UINT>(_countof(modelInputDesc)));

					shaderMap["Ramp"] = std::make_unique<RampShader>(device,
						"./Data/Shader/PhongBatchingVS.cso",// フォンシェーダーと同じ処理
						modelInputDesc, static_cast<UINT>(_countof(modelInputDesc)));

					shaderMap["Grass"] = std::make_unique<GrassShader>(device,
						"./Data/Shader/GrassVS.cso",
						"./Data/Shader/GrassGBPS.cso",
						modelInputDesc, static_cast<UINT>(_countof(modelInputDesc)));

					shaderMap["PBR"] = std::make_unique<PBRShader>(device,
						"./Data/Shader/PhongBatchingVS.cso",
						"./Data/Shader/PhysicalBasedRenderingGBPS.cso",
						modelInputDesc, static_cast<UINT>(_countof(modelInputDesc)));
				}
				{
					// InstancingModel
					const size_t type = static_cast<int>(ModelRenderType::Instancing);
					ShaderMap& shaderMap = deferredShaders_[type];

					shaderMap["Phong"] = std::make_unique<PhongShader>(device,
						"./Data/Shader/PhongInstancedVS.cso",
						"./Data/Shader/PhongGBPS.cso",
						modelInputDesc, static_cast<UINT>(_countof(modelInputDesc)));

					shaderMap["Ramp"] = std::make_unique<RampShader>(device,
						"./Data/Shader/PhongInstancedVS.cso",// フォンシェーダーと同じ処理
						modelInputDesc, static_cast<UINT>(_countof(modelInputDesc)));
				}
			}

			// フォワードレンダリング用
			{
				{
					// DynamicBoneModel
					const size_t type = static_cast<int>(ModelRenderType::Dynamic);
					ShaderMap& shaderMap = forwardShaders_[type];

					shaderMap["Phong"] = std::make_unique<PhongShader>(device,
						"./Data/Shader/PhongVS.cso",
						"./Data/Shader/PhongPS.cso",
						modelInputDesc, static_cast<UINT>(_countof(modelInputDesc)));
					shaderMap["PBR"] = std::make_unique<PBRShader>(device,
						"./Data/Shader/PhongVS.cso",
						"./Data/Shader/PhysicalBasedRenderingPS.cso",
						modelInputDesc, static_cast<UINT>(_countof(modelInputDesc)));
				}
				{
					// StaticBoneModel
					const size_t type = static_cast<int>(ModelRenderType::Static);
					ShaderMap& shaderMap = forwardShaders_[type];

					shaderMap["Phong"] = std::make_unique<PhongShader>(device,
						"./Data/Shader/PhongBatchingVS.cso",
						"./Data/Shader/PhongPS.cso",
						modelInputDesc, static_cast<UINT>(_countof(modelInputDesc)));

					shaderMap["Grass"] = std::make_unique<GrassShader>(device,
						"./Data/Shader/GrassVS.cso",
						"./Data/Shader/GrassPS.cso",
						modelInputDesc, static_cast<UINT>(_countof(modelInputDesc)));

					shaderMap["PBR"] = std::make_unique<PBRShader>(device,
						"./Data/Shader/PhongBatchingVS.cso",
						"./Data/Shader/PhysicalBasedRenderingPS.cso",
						modelInputDesc, static_cast<UINT>(_countof(modelInputDesc)));
				}
				{
					// InstancingModel
					const size_t type = static_cast<int>(ModelRenderType::Instancing);
					ShaderMap& shaderMap = forwardShaders_[type];

					shaderMap["Phong"] = std::make_unique<PhongShader>(device,
						"./Data/Shader/PhongInstancedVS.cso",
						"./Data/Shader/PhongPS.cso",
						modelInputDesc, static_cast<UINT>(_countof(modelInputDesc)));
				}
			}

			// カスケードシャドウマップ用
			cascadedSMShader_[static_cast<int>(ModelRenderType::Dynamic)] = 
				std::make_unique<CascadedShadowMapShader>(device,
					"./Data/Shader/CascadedShadowVS.cso",
					modelInputDesc, static_cast<UINT>(_countof(modelInputDesc)));
			cascadedSMShader_[static_cast<int>(ModelRenderType::Static)] = 
				std::make_unique<CascadedShadowMapShader>(device,
					"./Data/Shader/CascadedShadowBatchingVS.cso",
					modelInputDesc, static_cast<UINT>(_countof(modelInputDesc)));
			cascadedSMShader_[static_cast<int>(ModelRenderType::Instancing)] =
				std::make_unique<CascadedShadowMapShader>(device,
					"./Data/Shader/CascadedShadowInstancedVS.cso",
					modelInputDesc, static_cast<UINT>(_countof(modelInputDesc)));
		}
	}

	/// メッシュ描画
	void Draw(const ModelResource::Mesh* mesh,
		Model* model,
		const Vector4& color, 
		std::string shaderId,
		ModelRenderType renderType,
		ShaderBase::Parameter* parameter)
	{
		const float alpha = model->GetResource()->GetMaterials().at(mesh->materialIndex).colors.at("Diffuse").w;
		// 描画タイプに応じて登録
		switch (renderType)
		{
		case ModelRenderType::Dynamic:
			if (color.w < 1.0f || alpha < 1.0f)
				alphaDrawInfomap_.push_back({ model, mesh, color, parameter, shaderId, renderType, 0.0f });
			else
				dynamicInfomap_[shaderId].push_back({ model, mesh, color, parameter });
			break;
		case ModelRenderType::Static:
			if (color.w < 1.0f || alpha < 1.0f)
				alphaDrawInfomap_.push_back({ model, mesh, color, parameter,shaderId, renderType, 0.0f });
			else
				staticInfomap_[shaderId].push_back({ model, mesh, color, parameter });
			break;
		case ModelRenderType::Instancing:
			assert(!"Please Call \"DrawInstancing\"");
			break;
		default:
			assert(!"ModelRenderType Overflow");
			break;
		}
	}

	void DrawInstancing(Model* model, 
		const Vector4& color, 
		std::string shaderId,
		const DirectX::XMFLOAT4X4& world,
		ShaderBase::Parameter* parameter)
	{
		// 過去に登録しているか確認
		auto iter = instancingInfoMap_.find(model);

		InstancingDrawInfo::ModelParameter modelParameter{ color, world };
		if (iter != instancingInfoMap_.end())
		{
			// 既に登録している場合はパラメータを追加
			(*iter).second.modelParameters.push_back(modelParameter);
		}
		else
		{
			// ない場合は新規で登録
			instancingInfoMap_[model].parameter = parameter;
			instancingInfoMap_[model].shaderId = shaderId;
			instancingInfoMap_[model].modelParameters.push_back(modelParameter);
		}
	}

	/// 描画実行
	void RenderOpaque(const RenderContext& rc, bool writeGBuffer)
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
		if (writeGBuffer)
			dc->OMSetBlendState(rc.renderState->GetBlendState(BlendState::MultipleRenderTargets), nullptr, 0xFFFFFFFF);
		else
			dc->OMSetBlendState(rc.renderState->GetBlendState(BlendState::Opaque), nullptr, 0xFFFFFFFF);

		auto shaders = writeGBuffer ? deferredShaders_ : forwardShaders_;

		// ボーンの影響度があるモデルの描画
		{
			// 定数バッファ設定
			dc->VSSetConstantBuffers(1, 1, dynamicBoneCB_.GetAddressOf());
			dc->GSSetConstantBuffers(1, 1, dynamicBoneCB_.GetAddressOf());
			dc->PSSetConstantBuffers(1, 1, dynamicBoneCB_.GetAddressOf());

			// 不透明描画処理
			for (auto& drawInfomap : dynamicInfomap_)
			{
				ShaderBase* shader = shaders[static_cast<int>(ModelRenderType::Dynamic)][drawInfomap.first].get();
				assert(shader/*shadersに含まれないshaderを参照した*/);
				shader->Begin(rc);

				for (auto& drawInfo : drawInfomap.second)
				{
					// メッシュ描画
					Helper::DrawDynamicBoneMesh(rc, shader, drawInfo.model, drawInfo.mesh, drawInfo.color, drawInfo.parameter);
				}

				shader->End(rc);
			}
			dynamicInfomap_.clear();
		}

		// ボーンの影響度がないモデルの描画
		{
			ID3D11DeviceContext* dc = rc.deviceContext;
			// 定数バッファ設定
			dc->VSSetConstantBuffers(1, 1, staticBoneCB_.GetAddressOf());
			dc->GSSetConstantBuffers(1, 1, staticBoneCB_.GetAddressOf());
			dc->PSSetConstantBuffers(1, 1, staticBoneCB_.GetAddressOf());

			// 不透明描画処理
			for (auto& drawInfomap : staticInfomap_)
			{
				ShaderBase* shader = shaders[static_cast<int>(ModelRenderType::Static)][drawInfomap.first].get();
				assert(shader/*shadersに含まれないshaderを参照した*/);
				shader->Begin(rc);

				for (auto& drawInfo : drawInfomap.second)
				{
					// メッシュ描画
					Helper::DrawStaticBoneModel(rc, shader, drawInfo.model, drawInfo.mesh, drawInfo.color, drawInfo.parameter);
				}

				shader->End(rc);
			}
			staticInfomap_.clear();
		}

		// インスタンシングモデルの描画
		Helper::RenderInstancing(rc);
	}

	/// 半透明描画
	void RenderAlpha(const RenderContext& rc)
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

		// カメラ距離でソート
		DirectX::XMVECTOR CameraPosition = DirectX::XMLoadFloat3(&rc.camera->eye);
		DirectX::XMVECTOR CameraFront = DirectX::XMLoadFloat3(&rc.camera->front);
		for (auto& drawInfo : alphaDrawInfomap_)
		{
			const std::vector<ModelResource::Node>& nodes = drawInfo.model->GetPoseNodes();
			const ModelResource::Mesh* mesh = drawInfo.mesh;
			DirectX::XMVECTOR Position = DirectX::XMVectorSet(
				nodes[mesh->nodeIndex].worldTransform._41,
				nodes[mesh->nodeIndex].worldTransform._42,
				nodes[mesh->nodeIndex].worldTransform._43,
				0.0f);
			DirectX::XMVECTOR Vec = DirectX::XMVectorSubtract(Position, CameraPosition);
			drawInfo.distance = DirectX::XMVectorGetX(DirectX::XMVector3Dot(CameraFront, Vec));
		}
		std::sort(alphaDrawInfomap_.begin(), alphaDrawInfomap_.end(),
			[](const AlphaDrawInfo& lhs, const AlphaDrawInfo& rhs)
			{
				return lhs.distance > rhs.distance;
			});

		auto shaders = forwardShaders_;

		for (auto& drawInfo : alphaDrawInfomap_)
		{
			switch (drawInfo.renderType)
			{
			case ModelRenderType::Dynamic:
			{
				// 定数バッファ設定
				dc->VSSetConstantBuffers(1, 1, dynamicBoneCB_.GetAddressOf());
				dc->GSSetConstantBuffers(1, 1, dynamicBoneCB_.GetAddressOf());
				dc->PSSetConstantBuffers(1, 1, dynamicBoneCB_.GetAddressOf());

				ShaderBase* shader = shaders[static_cast<int>(ModelRenderType::Dynamic)][drawInfo.shaderID].get();
				assert(shader/*shadersに含まれないshaderを参照した*/);
				shader->Begin(rc);

				// メッシュ描画
				Helper::DrawDynamicBoneMesh(rc, shader, drawInfo.model, drawInfo.mesh, drawInfo.color, drawInfo.parameter);

				shader->End(rc);
				break;
			}
			case ModelRenderType::Static:
			{
				// 定数バッファ設定
				dc->VSSetConstantBuffers(1, 1, staticBoneCB_.GetAddressOf());
				dc->GSSetConstantBuffers(1, 1, staticBoneCB_.GetAddressOf());
				dc->PSSetConstantBuffers(1, 1, staticBoneCB_.GetAddressOf());

				ShaderBase* shader = shaders[static_cast<int>(ModelRenderType::Static)][drawInfo.shaderID].get();
				assert(shader/*shadersに含まれないshaderを参照した*/);
				shader->Begin(rc);

				// メッシュ描画
				Helper::DrawDynamicBoneMesh(rc, shader, drawInfo.model, drawInfo.mesh, drawInfo.color, drawInfo.parameter);

				shader->End(rc);
				break;
			}
			}
		}
		alphaDrawInfomap_.clear();
	}

	/// 影描画実行
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
			ShaderBase* shader = cascadedSMShader_[static_cast<int>(ModelRenderType::Dynamic)].get();
			shader->Begin(rc);
			for (auto& drawInfomap : dynamicInfomap_)
			{
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
						shader->Update(rc, &resource->GetMaterials().at(mesh.materialIndex), drawInfo.parameter);

						dc->DrawIndexedInstanced(static_cast<UINT>(mesh.indices.size()), CASCADED_SHADOW_MAPS_SIZE, 0, 0, 0);
					}
				}
			}
			shader->End(rc);
			dynamicInfomap_.clear();
		}

		// ボーンの影響度がないモデルの描画
		{
			ID3D11DeviceContext* dc = rc.deviceContext;
			// 定数バッファ設定
			dc->VSSetConstantBuffers(1, 1, staticBoneCB_.GetAddressOf());
			dc->PSSetConstantBuffers(1, 1, staticBoneCB_.GetAddressOf());

			// 不透明描画処理
			ShaderBase* shader = cascadedSMShader_[static_cast<int>(ModelRenderType::Static)].get();
			shader->Begin(rc);
			for (auto& drawInfomap : staticInfomap_)
			{
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
						shader->Update(rc, &resource->GetMaterials().at(mesh.materialIndex), drawInfo.parameter);

						dc->DrawIndexedInstanced(static_cast<UINT>(mesh.indices.size()), CASCADED_SHADOW_MAPS_SIZE, 0, 0, 0);
					}
				}
			}
			shader->End(rc);
			staticInfomap_.clear();
		}

		// インスタンシングモデルの描画
		//Helper::RenderInstancing(rc);
	}


	std::vector<const char*> GetShaderNames(ModelRenderType type, bool deferred)
	{
		std::vector<const char*> shaderNames;
		if (deferred)
		{
			for (auto& [name, shader] : deferredShaders_[static_cast<int>(type)])
			{
				shaderNames.push_back(name.c_str());
			}
		}
		else
		{
			for (auto& [name, shader] : forwardShaders_[static_cast<int>(type)])
			{
				shaderNames.push_back(name.c_str());
			}
		}
		return shaderNames;
	}

	ShaderBase::Parameter GetShaderParameterKey(ModelRenderType type, std::string key, bool deferred)
	{
		if (deferred)
		{
			return deferredShaders_[static_cast<int>(type)][key]->GetParameterKey();
		}
		else
		{
			return forwardShaders_[static_cast<int>(type)][key]->GetParameterKey();
		}
	}
}