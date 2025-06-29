#include "MeshRenderer.h"

#include "../Graphics/GpuResourceManager.h"
#include "../../Shader/Phong/PhongShader.h"
#include "../../Shader/Ramp/RampShader.h"
#include "../../Shader/Grass/GrassShader.h"
#include "../../Shader/PBR/PBRShader.h"

#include "../../Shader/CascadedShadowMap/CascadedShadowMapShader.h"

#include "../../Library/Algorithm/Converter.h"
#include "../../Library/DebugSupporter/DebugSupporter.h"

#include <algorithm>

/// 初期化
void MeshRenderer::Initialize(ID3D11Device* device)
{
	// 通常モデル用定数バッファ
	GpuResourceManager::CreateConstantBuffer(
		device,
		sizeof(DynamicBoneCB),
		_dynamicBoneCB.GetAddressOf());
	// アニメーションなしモデル用定数バッファ
	GpuResourceManager::CreateConstantBuffer(
		device,
		sizeof(StaticBoneCB),
		_staticBoneCB.GetAddressOf());
	// インスタンシングモデル用定数バッファ
	GpuResourceManager::CreateConstantBuffer(
		device,
		sizeof(InstancingModelCB),
		_instancingCB.GetAddressOf());

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
				ShaderMap& shaderMap = _deferredShaders[type];

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
				ShaderMap& shaderMap = _deferredShaders[type];

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
				ShaderMap& shaderMap = _deferredShaders[type];

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
				ShaderMap& shaderMap = _forwardShaders[type];

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
				ShaderMap& shaderMap = _forwardShaders[type];

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
				ShaderMap& shaderMap = _forwardShaders[type];

				shaderMap["Phong"] = std::make_unique<PhongShader>(device,
					"./Data/Shader/PhongInstancedVS.cso",
					"./Data/Shader/PhongPS.cso",
					modelInputDesc, static_cast<UINT>(_countof(modelInputDesc)));
			}
		}

		// カスケードシャドウマップ用
		_cascadedSMShader[static_cast<int>(ModelRenderType::Dynamic)] =
			std::make_unique<CascadedShadowMapShader>(device,
				"./Data/Shader/CascadedShadowVS.cso",
				modelInputDesc, static_cast<UINT>(_countof(modelInputDesc)));
		_cascadedSMShader[static_cast<int>(ModelRenderType::Static)] =
			std::make_unique<CascadedShadowMapShader>(device,
				"./Data/Shader/CascadedShadowBatchingVS.cso",
				modelInputDesc, static_cast<UINT>(_countof(modelInputDesc)));
		_cascadedSMShader[static_cast<int>(ModelRenderType::Instancing)] =
			std::make_unique<CascadedShadowMapShader>(device,
				"./Data/Shader/CascadedShadowInstancedVS.cso",
				modelInputDesc, static_cast<UINT>(_countof(modelInputDesc)));
	}
}

/// メッシュ描画
void MeshRenderer::Draw(const ModelResource::Mesh* mesh,
	Model* model,
	const Vector4& color,
	Material* material,
	ModelRenderType renderType,
	ShaderBase::Parameter* parameter)
{
	// 描画タイプに応じて登録
	switch (renderType)
	{
	case ModelRenderType::Dynamic:
		if (material->GetBlendType() == BlendType::Alpha)
			_alphaDrawInfomap.push_back({ model, mesh, color, material, parameter, renderType, 0.0f });
		else
			_dynamicInfomap[material->GetShaderName()].push_back({model, mesh, color, material, parameter});
		break;
	case ModelRenderType::Static:
		if (material->GetBlendType() == BlendType::Alpha)
			_alphaDrawInfomap.push_back({ model, mesh, color, material, parameter, renderType, 0.0f });
		else
			_staticInfomap[material->GetShaderName()].push_back({ model, mesh, color,  material, parameter });
		break;
	case ModelRenderType::Instancing:
		assert(!"Please Call \"DrawInstancing\"");
		break;
	default:
		assert(!"ModelRenderType Overflow");
		break;
	}
}

/// 影描画
void MeshRenderer::DrawShadow(
	const ModelResource::Mesh* mesh,
	Model* model,
	const Vector4& color,
	Material* material, 
	ModelRenderType renderType,
	ShaderBase::Parameter* parameter)
{
	// 描画タイプに応じて登録
	switch (renderType)
	{
	case ModelRenderType::Dynamic:
		_dynamicInfomap["CascadedShadowMap"].push_back({model, mesh, color, material, parameter});
		break;
	case ModelRenderType::Static:
		_staticInfomap["CascadedShadowMap"].push_back({model, mesh, color,  material, parameter});
		break;
	case ModelRenderType::Instancing:
		// TODO
		assert(!"Please Call \"DrawInstancing\"");
		break;
	default:
		assert(!"ModelRenderType Overflow");
		break;
	}
}

/// インスタンシングモデルの描画
void MeshRenderer::DrawInstancing(Model* model,
	const Vector4& color,
	Material* material,
	std::string shaderType,
	const DirectX::XMFLOAT4X4& world,
	ShaderBase::Parameter* parameter)
{
	// 過去に登録しているか確認
	auto iter = _instancingInfoMap.find(model);

	InstancingDrawInfo::ModelParameter modelParameter{ color, world };
	if (iter != _instancingInfoMap.end())
	{
		// 既に登録している場合はパラメータを追加
		(*iter).second.modelParameters.push_back(modelParameter);
	}
	else
	{
		// ない場合は新規で登録
		_instancingInfoMap[model].parameter = parameter;
		_instancingInfoMap[model].shaderType = shaderType;
		_instancingInfoMap[model].modelParameters.push_back(modelParameter);
		_instancingInfoMap[model].material = material;
	}
}

/// 描画実行
void MeshRenderer::RenderOpaque(const RenderContext& rc, bool writeGBuffer)
{
	ID3D11DeviceContext* dc = rc.deviceContext;

	// サンプラステート設定
	std::vector<ID3D11SamplerState*> samplerStates;
	for (size_t index = 0; index < static_cast<int>(SamplerState::EnumCount); ++index)
	{
		samplerStates.push_back(rc.renderState->GetSamplerState(static_cast<SamplerState>(index)));
	}
	dc->PSSetSamplers(0, static_cast<UINT>(samplerStates.size()), samplerStates.data());

	// レンダーステート設定
	dc->OMSetDepthStencilState(rc.renderState->GetDepthStencilState(DepthState::TestAndWrite), 0);
	dc->RSSetState(rc.renderState->GetRasterizerState(RasterizerState::SolidCullNone));

	// ブレンドステート設定
	if (writeGBuffer)
		dc->OMSetBlendState(rc.renderState->GetBlendState(BlendState::MultipleRenderTargets), nullptr, 0xFFFFFFFF);
	else
		dc->OMSetBlendState(rc.renderState->GetBlendState(BlendState::Opaque), nullptr, 0xFFFFFFFF);

	auto shaders = writeGBuffer ? _deferredShaders : _forwardShaders;

	// ボーンの影響度があるモデルの描画
	{
		// 定数バッファ設定
		dc->VSSetConstantBuffers(ModelCBIndex, 1, _dynamicBoneCB.GetAddressOf());
		dc->GSSetConstantBuffers(ModelCBIndex, 1, _dynamicBoneCB.GetAddressOf());
		dc->PSSetConstantBuffers(ModelCBIndex, 1, _dynamicBoneCB.GetAddressOf());

		// 不透明描画処理
		for (auto& drawInfomap : _dynamicInfomap)
		{
			ShaderBase* shader = shaders[static_cast<int>(ModelRenderType::Dynamic)][drawInfomap.first].get();
			// シェーダーがnullptrの場合はPhongシェーダーを使用
			if (shader == nullptr)
			{
				Debug::Output::String(L"\tRenderOpaque dynamicInfomap\n");
				Debug::Output::String(L"\tシェーダーがnullptrのためPhongシェーダーを使用\n");
				shader = shaders[static_cast<int>(ModelRenderType::Dynamic)]["Phong"].get();
			}
			shader->Begin(rc);

			for (auto& drawInfo : drawInfomap.second)
			{
				// メッシュ描画
				DrawDynamicBoneMesh(rc, shader, drawInfo);
			}

			shader->End(rc);
		}
		_dynamicInfomap.clear();
	}

	// ボーンの影響度がないモデルの描画
	{
		ID3D11DeviceContext* dc = rc.deviceContext;
		// 定数バッファ設定
		dc->VSSetConstantBuffers(ModelCBIndex, 1, _staticBoneCB.GetAddressOf());
		dc->GSSetConstantBuffers(ModelCBIndex, 1, _staticBoneCB.GetAddressOf());
		dc->PSSetConstantBuffers(ModelCBIndex, 1, _staticBoneCB.GetAddressOf());

		// 不透明描画処理
		for (auto& drawInfomap : _staticInfomap)
		{
			ShaderBase* shader = shaders[static_cast<int>(ModelRenderType::Static)][drawInfomap.first].get();
			// シェーダーがnullptrの場合はPhongシェーダーを使用
			if (shader == nullptr)
			{
				Debug::Output::String(L"\tRenderOpaque staticInfomap\n");
				Debug::Output::String(L"\tシェーダーがnullptrのためPhongシェーダーを使用\n");
				shader = shaders[static_cast<int>(ModelRenderType::Static)]["Phong"].get();
			}
			shader->Begin(rc);

			for (auto& drawInfo : drawInfomap.second)
			{
				// メッシュ描画
				DrawStaticBoneModel(rc, shader, drawInfo);
			}

			shader->End(rc);
		}
		_staticInfomap.clear();
	}

	// インスタンシングモデルの描画
	RenderInstancing(rc);
}

/// 半透明描画
void MeshRenderer::RenderAlpha(const RenderContext& rc)
{
	ID3D11DeviceContext* dc = rc.deviceContext;

	// サンプラステート設定
	std::vector<ID3D11SamplerState*> samplerStates;
	for (size_t index = 0; index < static_cast<int>(SamplerState::EnumCount); ++index)
	{
		samplerStates.push_back(rc.renderState->GetSamplerState(static_cast<SamplerState>(index)));
	}
	dc->PSSetSamplers(0, static_cast<UINT>(samplerStates.size()), samplerStates.data());

	// レンダーステート設定
	dc->OMSetDepthStencilState(rc.renderState->GetDepthStencilState(DepthState::TestAndWrite), 0);
	dc->RSSetState(rc.renderState->GetRasterizerState(RasterizerState::SolidCullNone));

	// ブレンドステート設定
	dc->OMSetBlendState(rc.renderState->GetBlendState(BlendState::Alpha), nullptr, 0xFFFFFFFF);

	// カメラ距離でソート
	DirectX::XMVECTOR CameraPosition = DirectX::XMLoadFloat3(&rc.camera->GetEye());
	DirectX::XMVECTOR CameraFront = DirectX::XMLoadFloat3(&rc.camera->GetFront());
	for (auto& alphaDrawInfo : _alphaDrawInfomap)
	{
		const std::vector<ModelResource::Node>& nodes = alphaDrawInfo.drawInfo.model->GetPoseNodes();
		const ModelResource::Mesh* mesh = alphaDrawInfo.drawInfo.mesh;
		DirectX::XMVECTOR Position = DirectX::XMVectorSet(
			nodes[mesh->nodeIndex].worldTransform._41,
			nodes[mesh->nodeIndex].worldTransform._42,
			nodes[mesh->nodeIndex].worldTransform._43,
			0.0f);
		DirectX::XMVECTOR Vec = DirectX::XMVectorSubtract(Position, CameraPosition);
		alphaDrawInfo.distance = DirectX::XMVectorGetX(DirectX::XMVector3Dot(CameraFront, Vec));
	}
	std::sort(_alphaDrawInfomap.begin(), _alphaDrawInfomap.end(),
		[](const AlphaDrawInfo& lhs, const AlphaDrawInfo& rhs)
		{
			return lhs.distance > rhs.distance;
		});

	auto shaders = _forwardShaders;

	for (auto& alphaDrawInfo : _alphaDrawInfomap)
	{
		switch (alphaDrawInfo.renderType)
		{
		case ModelRenderType::Dynamic:
		{
			// 定数バッファ設定
			dc->VSSetConstantBuffers(ModelCBIndex, 1, _dynamicBoneCB.GetAddressOf());
			dc->GSSetConstantBuffers(ModelCBIndex, 1, _dynamicBoneCB.GetAddressOf());
			dc->PSSetConstantBuffers(ModelCBIndex, 1, _dynamicBoneCB.GetAddressOf());

			ShaderBase* shader = shaders
				[static_cast<int>(ModelRenderType::Dynamic)]
				[alphaDrawInfo.drawInfo.material->GetShaderName()].get();
			// シェーダーがnullptrの場合はPhongシェーダーを使用
			if (shader == nullptr)
			{
				Debug::Output::String(L"\tRenderAlpha ModelRenderType::Dynamic\n");
				Debug::Output::String(L"\tシェーダーがnullptrのためPhongシェーダーを使用\n");
				shader = shaders[static_cast<int>(ModelRenderType::Dynamic)]["Phong"].get();
			}
			shader->Begin(rc);

			// メッシュ描画
			DrawDynamicBoneMesh(rc, shader, alphaDrawInfo.drawInfo);

			shader->End(rc);
			break;
		}
		case ModelRenderType::Static:
		{
			// 定数バッファ設定
			dc->VSSetConstantBuffers(ModelCBIndex, 1, _staticBoneCB.GetAddressOf());
			dc->GSSetConstantBuffers(ModelCBIndex, 1, _staticBoneCB.GetAddressOf());
			dc->PSSetConstantBuffers(ModelCBIndex, 1, _staticBoneCB.GetAddressOf());

			ShaderBase* shader = shaders
				[static_cast<int>(ModelRenderType::Static)]
				[alphaDrawInfo.drawInfo.material->GetShaderName()].get();
			// シェーダーがnullptrの場合はPhongシェーダーを使用
			if (shader == nullptr)
			{
				Debug::Output::String(L"\tRenderAlpha ModelRenderType::Static\n");
				Debug::Output::String(L"\tシェーダーがnullptrのためPhongシェーダーを使用\n");
				shader = shaders[static_cast<int>(ModelRenderType::Static)]["Phong"].get();
			}
			shader->Begin(rc);

			// メッシュ描画
			DrawDynamicBoneMesh(rc, shader, alphaDrawInfo.drawInfo);

			shader->End(rc);
			break;
		}
		}
	}
	_alphaDrawInfomap.clear();
}

/// 影描画実行
void MeshRenderer::CastShadow(const RenderContext& rc)
{
	ID3D11DeviceContext* dc = rc.deviceContext;

	// サンプラステート設定
	std::vector<ID3D11SamplerState*> samplerStates;
	for (size_t index = 0; index < static_cast<int>(SamplerState::EnumCount); ++index)
	{
		samplerStates.push_back(rc.renderState->GetSamplerState(static_cast<SamplerState>(index)));
	}
	dc->PSSetSamplers(0, static_cast<UINT>(samplerStates.size()), samplerStates.data());

	// レンダーステート設定
	dc->OMSetDepthStencilState(rc.renderState->GetDepthStencilState(DepthState::TestAndWrite), 0);
	dc->RSSetState(rc.renderState->GetRasterizerState(RasterizerState::SolidCullBack));
	// ブレンドステート設定
	dc->OMSetBlendState(rc.renderState->GetBlendState(BlendState::Transparency), nullptr, 0xFFFFFFFF);

	// ボーンの影響度があるモデルの描画
	{
		// 定数バッファ設定
		dc->VSSetConstantBuffers(ModelCBIndex, 1, _dynamicBoneCB.GetAddressOf());
		dc->PSSetConstantBuffers(ModelCBIndex, 1, _dynamicBoneCB.GetAddressOf());

		// 不透明描画処理
		ShaderBase* shader = _cascadedSMShader[static_cast<int>(ModelRenderType::Dynamic)].get();
		shader->Begin(rc);
		for (auto& drawInfomap : _dynamicInfomap)
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
							DirectX::XMStoreFloat4x4(&_cbDynamicSkeleton.boneTransforms[i], Bone);
						}
					}
					else
					{
						_cbDynamicSkeleton.boneTransforms[0] = nodes[mesh.nodeIndex].worldTransform;
					}
					_cbDynamicSkeleton.materialColor = color;

					dc->UpdateSubresource(_dynamicBoneCB.Get(), 0, 0, &_cbDynamicSkeleton, 0, 0);

					// シェーダーの更新処理
					shader->Update(rc, drawInfo.material, drawInfo.parameter);

					dc->DrawIndexedInstanced(static_cast<UINT>(mesh.indices.size()), _CASCADED_SHADOW_MAPS_SIZE, 0, 0, 0);
				}
			}
		}
		shader->End(rc);
		_dynamicInfomap.clear();
	}

	// ボーンの影響度がないモデルの描画
	{
		ID3D11DeviceContext* dc = rc.deviceContext;
		// 定数バッファ設定
		dc->VSSetConstantBuffers(ModelCBIndex, 1, _staticBoneCB.GetAddressOf());
		dc->PSSetConstantBuffers(ModelCBIndex, 1, _staticBoneCB.GetAddressOf());

		// 不透明描画処理
		ShaderBase* shader = _cascadedSMShader[static_cast<int>(ModelRenderType::Static)].get();
		shader->Begin(rc);
		for (auto& drawInfomap : _staticInfomap)
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
					_cbStaticSkeleton.world = nodes[mesh.nodeIndex].worldTransform;
					_cbStaticSkeleton.materialColor = color;
					dc->UpdateSubresource(_staticBoneCB.Get(), 0, 0, &_cbStaticSkeleton, 0, 0);

					// シェーダーの更新処理
					shader->Update(rc, drawInfo.material, drawInfo.parameter);

					dc->DrawIndexedInstanced(static_cast<UINT>(mesh.indices.size()), _CASCADED_SHADOW_MAPS_SIZE, 0, 0, 0);
				}
			}
		}
		shader->End(rc);
		_staticInfomap.clear();
	}

	// インスタンシングモデルの描画
	//Helper::RenderInstancing(rc);
}


/// ModelRenderTypeのシェーダー名を取得
std::vector<const char*> MeshRenderer::GetShaderNames(ModelRenderType type, bool deferred)
{
	std::vector<const char*> shaderTypes;
	if (deferred)
	{
		for (auto& [name, shader] : _deferredShaders[static_cast<int>(type)])
		{
			// shaderがnullptrの時はスキップ
			if (shader == nullptr)
				continue;
			shaderTypes.push_back(name.c_str());
		}
	}
	else
	{
		for (auto& [name, shader] : _forwardShaders[static_cast<int>(type)])
		{
			// shaderがnullptrの時はスキップ
			if (shader == nullptr)
				continue;
			shaderTypes.push_back(name.c_str());
		}
	}

	return shaderTypes;
}

/// typeとkeyからパラメータのkeyを取得
ShaderBase::Parameter MeshRenderer::GetShaderParameterKey(ModelRenderType type, std::string key, bool deferred)
{
	if (deferred)
	{
		return _deferredShaders[static_cast<int>(type)][key]->GetParameterKey();
	}
	else
	{
		return _forwardShaders[static_cast<int>(type)][key]->GetParameterKey();
	}
}

// インスタンシングモデルの描画
void MeshRenderer::RenderInstancing(const RenderContext& rc)
{
	ID3D11DeviceContext* dc = rc.deviceContext;
	// 定数バッファ設定
	dc->VSSetConstantBuffers(ModelCBIndex, 1, _instancingCB.GetAddressOf());
	dc->PSSetConstantBuffers(ModelCBIndex, 1, _instancingCB.GetAddressOf());

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
					_cbInstancingSkeleton.materialColor[modelCount] = color;
					_cbInstancingSkeleton.world[modelCount] = world;
					modelCount++;
				}
				dc->UpdateSubresource(_instancingCB.Get(), 0, 0, &_cbInstancingSkeleton, 0, 0);

				// シェーダーの更新処理
				shader->Update(rc, drawInfo.material, drawInfo.parameter);

				dc->DrawIndexedInstanced(static_cast<UINT>(mesh.indices.size()), static_cast<UINT>(modelCount), 0, 0, 0);
			}
		};

	// 不透明描画処理
	for (auto& drawInfomap : _instancingInfoMap)
	{
		Model* model = drawInfomap.first;
		InstancingDrawInfo& drawInfo = drawInfomap.second;

		ShaderBase* shader = _deferredShaders[static_cast<int>(ModelRenderType::Instancing)][drawInfo.shaderType].get();
		shader->Begin(rc);

		DrawModel(model, drawInfo, shader);

		shader->End(rc);
	}
	_instancingInfoMap.clear();
}

// DynamicBoneModelのメッシュ描画
void MeshRenderer::DrawDynamicBoneMesh(const RenderContext& rc, ShaderBase* shader, DrawInfo& drawInfo)
{
	ID3D11DeviceContext*		dc = rc.deviceContext;
	Model*						model = drawInfo.model;
	const ModelResource::Mesh*	mesh = drawInfo.mesh;
	const Vector4&				materialColor = drawInfo.color;
	Material*					material = drawInfo.material;
	ShaderBase::Parameter*		parameter = drawInfo.parameter;
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
			DirectX::XMStoreFloat4x4(&_cbDynamicSkeleton.boneTransforms[i], Bone);
		}
	}
	else
	{
		_cbDynamicSkeleton.boneTransforms[0] = nodes[mesh->nodeIndex].worldTransform;
	}
	_cbDynamicSkeleton.materialColor = materialColor;

	dc->UpdateSubresource(_dynamicBoneCB.Get(), 0, 0, &_cbDynamicSkeleton, 0, 0);

	// シェーダーの更新処理
	shader->Update(rc, material, parameter);

	dc->DrawIndexed(static_cast<UINT>(mesh->indices.size()), 0, 0);
}

// StaticBoneModelのメッシュ描画
void MeshRenderer::DrawStaticBoneModel(const RenderContext& rc, ShaderBase* shader, DrawInfo& drawInfo)
{
	ID3D11DeviceContext*		dc = rc.deviceContext;
	Model*						model = drawInfo.model;
	const ModelResource::Mesh*	mesh = drawInfo.mesh;
	const Vector4&				materialColor = drawInfo.color;
	Material*					material = drawInfo.material;
	ShaderBase::Parameter*		parameter = drawInfo.parameter;
	const ModelResource*		resource = model->GetResource();
	const std::vector<ModelResource::Node>& nodes = model->GetPoseNodes();

	uint32_t stride{ sizeof(ModelResource::Vertex) };
	uint32_t offset{ 0 };
	dc->IASetVertexBuffers(0, 1, mesh->vertexBuffer.GetAddressOf(), &stride, &offset);
	dc->IASetIndexBuffer(mesh->indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// スケルトン用定数バッファ
	_cbStaticSkeleton.world = nodes[mesh->nodeIndex].worldTransform;
	_cbStaticSkeleton.materialColor = materialColor;
	dc->UpdateSubresource(_staticBoneCB.Get(), 0, 0, &_cbStaticSkeleton, 0, 0);

	// シェーダーの更新処理
	shader->Update(rc, material, parameter);

	dc->DrawIndexed(static_cast<UINT>(mesh->indices.size()), 0, 0);
}
