#include "MeshRenderer.h"

#include "../Graphics/GpuResourceManager.h"
#include "../../Library/JobSystem/JobSystem.h"
#include "../../Shader/Model/CascadedShadowMap/CascadedShadowMapShader.h"

#include "../../Library/Algorithm/Converter.h"
#include "../../Library/DebugSupporter/DebugSupporter.h"

#include "../../Library/HRTrace.h"

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

	// リソースを初期化
	auto modelShaderResource = ResourceManager::Instance().GetResourceAs<ModelShaderResource>();
	modelShaderResource->Initialize(device);

	// インスタンシング描画用バッファ作成
	{
		D3D11_BUFFER_DESC desc{};
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.ByteWidth = sizeof(InstancingDrawInfo::Data) * INSTANCED_MAX;
		desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
		desc.StructureByteStride = sizeof(InstancingDrawInfo::Data);
		desc.Usage = D3D11_USAGE_DEFAULT;
		HRESULT hr = device->CreateBuffer(&desc, nullptr, _instancingDataBuffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

		hr = device->CreateShaderResourceView(_instancingDataBuffer.Get(),
			nullptr, _instancingDataSRV.ReleaseAndGetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	}

	_testMaterial.SetShaderName("Test");
}

/// メッシュ描画
void MeshRenderer::Draw(const ModelResource::Mesh* mesh,
	Model* model,
	const Vector4& color,
	Material* material,
	ModelRenderType renderType)
{
	// 描画タイプに応じて登録
	switch (renderType)
	{
	case ModelRenderType::Dynamic:
		if (material->GetBlendType() == BlendType::Alpha)
			_alphaDrawInfomap.push_back({ model, mesh, color, material, renderType, 0.0f });
		else
			_dynamicInfomap[material->GetShaderName()].push_back({ model, mesh, color, material });
		break;
	case ModelRenderType::Static:
		if (material->GetBlendType() == BlendType::Alpha)
			_alphaDrawInfomap.push_back({ model, mesh, color, material, renderType, 0.0f });
		else
			_staticInfomap[material->GetShaderName()].push_back({ model, mesh, color,  material });
		break;
	case ModelRenderType::Instancing:
		assert(!"Please Call \"DrawInstancing\"");
		break;
	default:
		assert(!"ModelRenderType Overflow");
		break;
	}
}

/// メッシュのテスト描画
void MeshRenderer::DrawTest(const ModelResource::Mesh* mesh, Model* model, ModelRenderType renderType)
{
	MeshRenderer::Draw(mesh, model, Vector4::White, &_testMaterial, renderType);
}
/// メッシュのテスト描画
void MeshRenderer::DrawTest(Model* model, const DirectX::XMFLOAT4X4& world)
{
	MeshRenderer::DrawInstancing(model, Vector4::White, &_testMaterial, world);
}

/// 影描画
void MeshRenderer::DrawShadow(
	const ModelResource::Mesh* mesh,
	Model* model,
	const Vector4& color,
	Material* material,
	ModelRenderType renderType)
{
	// 描画タイプに応じて登録
	switch (renderType)
	{
	case ModelRenderType::Dynamic:
		_dynamicInfomap["CascadedShadowMap"].push_back({ model, mesh, color, material });
		break;
	case ModelRenderType::Static:
		_staticInfomap["CascadedShadowMap"].push_back({ model, mesh, color,  material });
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
	const DirectX::XMFLOAT4X4& world)
{
	std::string key = material->GetShaderName() + model->GetFilename();

	// 過去に登録しているか確認
	if (_instancingInfoMap.find(key) == _instancingInfoMap.end())
	{
		// 要素がないならば新規登録
		_instancingInfoMap[key].model = model;
		_instancingInfoMap[key].material = material;
	}
	// パラメータを追加
	InstancingDrawInfo::Data modelData{ world, color };
	_instancingInfoMap[key].modelDatas.push_back(modelData);
}

/// 描画実行
void MeshRenderer::RenderOpaque(const RenderContext& rc, bool writeGBuffer)
{
	ProfileScopedSection_3(0, "MeshRenderer::RenderOpaque", ImGuiControl::Profiler::Green);

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

	auto modelShaderResource = ResourceManager::Instance().GetResourceAs<ModelShaderResource>();
	auto shaders = modelShaderResource->GetShaderMap(writeGBuffer);

	// ボーンの影響度があるモデルの描画
	{
		// 定数バッファ設定
		dc->VSSetConstantBuffers(ModelCBIndex, 1, _dynamicBoneCB.GetAddressOf());
		dc->GSSetConstantBuffers(ModelCBIndex, 1, _dynamicBoneCB.GetAddressOf());
		dc->PSSetConstantBuffers(ModelCBIndex, 1, _dynamicBoneCB.GetAddressOf());

		// 不透明描画処理
		for (auto& drawInfomap : _dynamicInfomap)
		{
			auto* shader = shaders[static_cast<int>(ModelRenderType::Dynamic)][drawInfomap.first].get();
			// シェーダーがnullptrの場合はPhongシェーダーを使用
			if (shader == nullptr)
			{
				Debug::Output::String(L"\tRenderOpaque dynamicInfomap\n");
				Debug::Output::String(L"\tシェーダーがnullptrのためPhongシェーダーを使用\n");
				shader = shaders[static_cast<int>(ModelRenderType::Dynamic)]["Phong"].get();
			}
			ProfileScopedSection_3(0, drawInfomap.first.c_str(), ImGuiControl::Profiler::Blue);

			shader->Begin(rc);

			for (auto& drawInfo : drawInfomap.second)
			{
				ProfileScopedSection_3(0, 
					drawInfo.material ? drawInfo.material->GetName().c_str() : "NoMaterialMesh",
					ImGuiControl::Profiler::Yellow);

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
			auto* shader = shaders[static_cast<int>(ModelRenderType::Static)][drawInfomap.first].get();
			// シェーダーがnullptrの場合はPhongシェーダーを使用
			if (shader == nullptr)
			{
				Debug::Output::String(L"\tRenderOpaque staticInfomap\n");
				Debug::Output::String(L"\tシェーダーがnullptrのためPhongシェーダーを使用\n");
				shader = shaders[static_cast<int>(ModelRenderType::Static)]["Phong"].get();
			}
			ProfileScopedSection_3(0, drawInfomap.first.c_str(), ImGuiControl::Profiler::Blue);
			shader->Begin(rc);

			for (auto& drawInfo : drawInfomap.second)
			{
				ProfileScopedSection_3(0,
					drawInfo.material ? drawInfo.material->GetName().c_str() : "NoMaterialMesh",
					ImGuiControl::Profiler::Yellow);

				// メッシュ描画
				DrawStaticBoneModel(rc, shader, drawInfo);
			}

			shader->End(rc);
		}
		_staticInfomap.clear();
	}
}

/// 半透明描画
void MeshRenderer::RenderAlpha(const RenderContext& rc)
{
	ProfileScopedSection_3(0, "MeshRenderer::RenderAlpha", ImGuiControl::Profiler::Green);

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

	{
		ProfileScopedSection_3(0, "Sort", ImGuiControl::Profiler::Green);

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

		// 遠い順にソート
		std::sort(_alphaDrawInfomap.begin(), _alphaDrawInfomap.end(),
			[](const AlphaDrawInfo& lhs, const AlphaDrawInfo& rhs)
			{
				return lhs.distance > rhs.distance;
			});
	}

	auto modelShaderResource = ResourceManager::Instance().GetResourceAs<ModelShaderResource>();
	auto shaders = modelShaderResource->GetShaderMap(false);

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

			auto* shader = shaders
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

			ProfileScopedSection_3(0,
				alphaDrawInfo.drawInfo.material ? alphaDrawInfo.drawInfo.material->GetName().c_str() : "NoMaterialMesh",
				ImGuiControl::Profiler::Yellow);
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

			auto* shader = shaders
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

			ProfileScopedSection_3(0,
				alphaDrawInfo.drawInfo.material ? alphaDrawInfo.drawInfo.material->GetName().c_str() : "NoMaterialMesh",
				ImGuiControl::Profiler::Yellow);
			// メッシュ描画
			DrawDynamicBoneMesh(rc, shader, alphaDrawInfo.drawInfo);

			shader->End(rc);
			break;
		}
		}
	}
	_alphaDrawInfomap.clear();

	// インスタンシングモデルの描画
	RenderInstancing(rc);
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
		auto modelShaderResource = ResourceManager::Instance().GetResourceAs<ModelShaderResource>();
		auto* shader = modelShaderResource->GetCascadedSMShader(ModelRenderType::Dynamic);
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
					shader->Update(rc, drawInfo.material);

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
		auto modelShaderResource = ResourceManager::Instance().GetResourceAs<ModelShaderResource>();
		auto* shader = modelShaderResource->GetCascadedSMShader(ModelRenderType::Static);
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
					shader->Update(rc, drawInfo.material);

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

// インスタンシングモデルの描画
void MeshRenderer::RenderInstancing(const RenderContext& rc)
{
	ProfileScopedSection_3(0, "MeshRenderer::RenderInstancing", ImGuiControl::Profiler::Blue);

	ID3D11DeviceContext* dc = rc.deviceContext;

	// モデルの描画関数
	auto DrawModel = [&](InstancingDrawInfo& drawInfo, ModelShaderBase* shader)->void
		{
			Model* model = drawInfo.model;
			const ModelResource* resource = model->GetResource();
			const std::vector<ModelResource::Node>& nodes = model->GetPoseNodes();

			if (drawInfo.modelDatas.size() > INSTANCED_MAX)
			{
				// インスタンス数が多い場合はカメラの距離でソートして描画
				// key : モデルのインデックス, value : カメラからの距離
				std::vector<std::pair<size_t, float>> distanceMap;
				{
					DirectX::XMVECTOR CameraPosition = DirectX::XMLoadFloat3(&rc.camera->GetEye());
					size_t index = 0;
					for (auto& [world, color] : drawInfo.modelDatas)
					{
						DirectX::XMVECTOR Position = DirectX::XMVectorSet(world._41, world._42, world._43, 0.0f);
						DirectX::XMVECTOR Vec = DirectX::XMVectorSubtract(Position, CameraPosition);
						distanceMap.push_back(std::make_pair(index, DirectX::XMVectorGetX(DirectX::XMVector3LengthSq(Vec))));
						index++;
					}
					// 近い順にソート
					std::sort(distanceMap.begin(), distanceMap.end(),
						[](const std::pair<size_t, float>& lhs, const std::pair<size_t, float>& rhs)
						{
							return lhs.second < rhs.second;
						});
				}
				std::vector<InstancingDrawInfo::Data> temp;
				for (size_t i = 0; i < INSTANCED_MAX; ++i)
				{
					temp.push_back(drawInfo.modelDatas[distanceMap[i].first]);
				}
				drawInfo.modelDatas.clear();
				drawInfo.modelDatas = temp;
			}

			// モデルデータ更新
			D3D11_BOX writeBox = {};
			writeBox.left = 0;
			writeBox.right = static_cast<UINT>(drawInfo.modelDatas.size() * sizeof(InstancingDrawInfo::Data));
			writeBox.top = 0;
			writeBox.bottom = 1;
			writeBox.front = 0;
			writeBox.back = 1;
			dc->UpdateSubresource(
				_instancingDataBuffer.Get(),
				0,
				&writeBox,
				drawInfo.modelDatas.data(),
				writeBox.right,
				0);
			dc->VSSetShaderResources(10, 1, _instancingDataSRV.GetAddressOf());

			for (const ModelResource::Mesh& mesh : resource->GetMeshes())
			{
				uint32_t stride{ sizeof(ModelResource::Vertex) };
				uint32_t offset{ 0 };
				dc->IASetVertexBuffers(0, 1, mesh.vertexBuffer.GetAddressOf(), &stride, &offset);
				dc->IASetIndexBuffer(mesh.indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
				dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

				// シェーダーの更新処理
				shader->Update(rc, drawInfo.material);

				dc->DrawIndexedInstanced(static_cast<UINT>(mesh.indices.size()), static_cast<UINT>(drawInfo.modelDatas.size()), 0, 0, 0);
			}
		};

	// 描画処理
	for (auto& [shaderName, drawInfo] : _instancingInfoMap)
	{
		auto modelShaderResource = ResourceManager::Instance().GetResourceAs<ModelShaderResource>();
		auto* shader = modelShaderResource->GetShaderMap(false)[static_cast<int>(ModelRenderType::Instancing)][drawInfo.material->GetShaderName()].get();
		shader->Begin(rc);

		DrawModel(drawInfo, shader);

		shader->End(rc);
	}
	_instancingInfoMap.clear();
}

// DynamicBoneModelのメッシュ描画
void MeshRenderer::DrawDynamicBoneMesh(const RenderContext& rc, ModelShaderBase* shader, DrawInfo& drawInfo)
{
	ID3D11DeviceContext* dc = rc.deviceContext;
	Model* model = drawInfo.model;
	const ModelResource::Mesh* mesh = drawInfo.mesh;
	const Vector4& materialColor = drawInfo.color;
	Material* material = drawInfo.material;
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
	shader->Update(rc, material);

	dc->DrawIndexed(static_cast<UINT>(mesh->indices.size()), 0, 0);
}

// StaticBoneModelのメッシュ描画
void MeshRenderer::DrawStaticBoneModel(const RenderContext& rc, ModelShaderBase* shader, DrawInfo& drawInfo)
{
	ID3D11DeviceContext* dc = rc.deviceContext;
	Model* model = drawInfo.model;
	const ModelResource::Mesh* mesh = drawInfo.mesh;
	const Vector4& materialColor = drawInfo.color;
	Material* material = drawInfo.material;
	const ModelResource* resource = model->GetResource();
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
	shader->Update(rc, material);

	dc->DrawIndexed(static_cast<UINT>(mesh->indices.size()), 0, 0);
}
