#include "ModelShaderResource.h"

#include "../../Shader/Model/Phong/PhongShader.h"
#include "../../Shader/Model/Ramp/RampShader.h"
#include "../../Shader/Model/Grass/GrassShader.h"
#include "../../Shader/Model/PBR/PBRShader.h"
#include "../../Shader/Model/Test/TestShader.h"
#include "../../Shader/Model/Player/PlayerShader.h"

#include "../../Shader/Model/CascadedShadowMap/CascadedShadowMapShader.h"

#include <imgui.h>

// 初期化処理
void ModelShaderResource::Initialize(ID3D11Device* device)
{
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
					"./Data/Shader/HLSL/Model/Phong/PhongVS.cso",
					"./Data/Shader/HLSL/Model/Phong/PhongGBPS.cso",
					modelInputDesc, static_cast<UINT>(_countof(modelInputDesc)));

				shaderMap["Ramp"] = std::make_unique<RampShader>(device,
					"./Data/Shader/HLSL/Model/Phong/PhongVS.cso",// フォンシェーダーと同じ処理
					modelInputDesc, static_cast<UINT>(_countof(modelInputDesc)));

				shaderMap["PBR"] = std::make_unique<PBRShader>(device,
					"./Data/Shader/HLSL/Model/Phong/PhongVS.cso",// フォンシェーダーと同じ処理
					"./Data/Shader/HLSL/Model/PhysicalBasedRendering/PhysicalBasedRenderingGBPS.cso",
					modelInputDesc, static_cast<UINT>(_countof(modelInputDesc)));

				shaderMap["Player"] = std::make_unique<PlayerShader>(device,
					modelInputDesc, static_cast<UINT>(_countof(modelInputDesc)));

				shaderMap["Test"] = std::make_unique<TestShader>(device,
					"./Data/Shader/HLSL/Model/Phong/PhongVS.cso",// フォンシェーダーと同じ処理
					"./Data/Shader/HLSL/Model/Test/TestGBPS.cso",
					modelInputDesc, static_cast<UINT>(_countof(modelInputDesc)));
			}
			{
				// StaticBoneModel
				const size_t type = static_cast<int>(ModelRenderType::Static);
				ShaderMap& shaderMap = _deferredShaders[type];

				shaderMap["Phong"] = std::make_unique<PhongShader>(device,
					"./Data/Shader/HLSL/Model/Phong/PhongBatchingVS.cso",
					"./Data/Shader/HLSL/Model/Phong/PhongGBPS.cso",
					modelInputDesc, static_cast<UINT>(_countof(modelInputDesc)));

				shaderMap["Ramp"] = std::make_unique<RampShader>(device,
					"./Data/Shader/HLSL/Model/Phong/PhongBatchingVS.cso",// フォンシェーダーと同じ処理
					modelInputDesc, static_cast<UINT>(_countof(modelInputDesc)));

				shaderMap["PBR"] = std::make_unique<PBRShader>(device,
					"./Data/Shader/HLSL/Model/Phong/PhongBatchingVS.cso",// フォンシェーダーと同じ処理
					"./Data/Shader/HLSL/Model/PhysicalBasedRendering/PhysicalBasedRenderingGBPS.cso",
					modelInputDesc, static_cast<UINT>(_countof(modelInputDesc)));

				shaderMap["Test"] = std::make_unique<TestShader>(device,
					"./Data/Shader/HLSL/Model/Phong/PhongBatchingVS.cso",// フォンシェーダーと同じ処理
					"./Data/Shader/HLSL/Model/Test/TestGBPS.cso",
					modelInputDesc, static_cast<UINT>(_countof(modelInputDesc)));
			}
			{
				// InstancingModel
				// デファードでは描画しない
			}
		}

		// フォワードレンダリング用
		{
			{
				// DynamicBoneModel
				const size_t type = static_cast<int>(ModelRenderType::Dynamic);
				ShaderMap& shaderMap = _forwardShaders[type];

				shaderMap["Phong"] = std::make_unique<PhongShader>(device,
					"./Data/Shader/HLSL/Model/Phong/PhongVS.cso",
					"./Data/Shader/HLSL/Model/Phong/PhongPS.cso",
					modelInputDesc, static_cast<UINT>(_countof(modelInputDesc)));

				shaderMap["PBR"] = std::make_unique<PBRShader>(device,
					"./Data/Shader/HLSL/Model/Phong/PhongVS.cso",
					"./Data/Shader/HLSL/Model/PhysicalBasedRendering/PhysicalBasedRenderingPS.cso",
					modelInputDesc, static_cast<UINT>(_countof(modelInputDesc)));

				shaderMap["Test"] = std::make_unique<TestShader>(device,
					"./Data/Shader/HLSL/Model/Phong/PhongVS.cso",// フォンシェーダーと同じ処理
					"./Data/Shader/HLSL/Model/Test/TestPS.cso",
					modelInputDesc, static_cast<UINT>(_countof(modelInputDesc)));
			}
			{
				// StaticBoneModel
				const size_t type = static_cast<int>(ModelRenderType::Static);
				ShaderMap& shaderMap = _forwardShaders[type];

				shaderMap["Phong"] = std::make_unique<PhongShader>(device,
					"./Data/Shader/HLSL/Model/Phong/PhongBatchingVS.cso",
					"./Data/Shader/HLSL/Model/Phong/PhongPS.cso",
					modelInputDesc, static_cast<UINT>(_countof(modelInputDesc)));
				shaderMap["PhongAlpha"] = std::make_unique<PhongShader>(device,
					"./Data/Shader/HLSL/Model/Phong/PhongBatchingVS.cso",
					"./Data/Shader/HLSL/Model/Phong/PhongAlphaPS.cso",
					modelInputDesc, static_cast<UINT>(_countof(modelInputDesc)));

				shaderMap["PBR"] = std::make_unique<PBRShader>(device,
					"./Data/Shader/HLSL/Model/Phong/PhongBatchingVS.cso",
					"./Data/Shader/HLSL/Model/PhysicalBasedRendering/PhysicalBasedRenderingPS.cso",
					modelInputDesc, static_cast<UINT>(_countof(modelInputDesc)));
				shaderMap["PBRAlpha"] = std::make_unique<PBRShader>(device,
					"./Data/Shader/HLSL/Model/Phong/PhongBatchingVS.cso",
					"./Data/Shader/HLSL/Model/PhysicalBasedRendering/PhysicalBasedRenderingAlphaPS.cso",
					modelInputDesc, static_cast<UINT>(_countof(modelInputDesc)));

				shaderMap["Test"] = std::make_unique<TestShader>(device,
					"./Data/Shader/HLSL/Model/Phong/PhongBatchingVS.cso",// フォンシェーダーと同じ処理
					"./Data/Shader/HLSL/Model/Test/TestPS.cso",
					modelInputDesc, static_cast<UINT>(_countof(modelInputDesc)));
			}
			{
				// InstancingModel
				const size_t type = static_cast<int>(ModelRenderType::Instancing);
				ShaderMap& shaderMap = _forwardShaders[type];

				shaderMap["Phong"] = std::make_unique<PhongShader>(device,
					"./Data/Shader/HLSL/Model/Phong/PhongInstancedVS.cso",
					"./Data/Shader/HLSL/Model/Phong/PhongPS.cso",
					modelInputDesc, static_cast<UINT>(_countof(modelInputDesc)));
				shaderMap["PhongAlpha"] = std::make_unique<PhongShader>(device,
					"./Data/Shader/HLSL/Model/Phong/PhongInstancedVS.cso",
					"./Data/Shader/HLSL/Model/Phong/PhongAlphaPS.cso",
					modelInputDesc, static_cast<UINT>(_countof(modelInputDesc)));
				shaderMap["Test"] = std::make_unique<TestShader>(device,
					"./Data/Shader/HLSL/Model/Phong/PhongInstancedVS.cso",
					"./Data/Shader/HLSL/Model/Test/TestPS.cso",
					modelInputDesc, static_cast<UINT>(_countof(modelInputDesc)));
				shaderMap["Grass"] = std::make_unique<GrassShader>(device,
					"./Data/Shader/HLSL/Model/Grass/GrassInstancedVS.cso",
					"./Data/Shader/HLSL/Model/Phong/PhongAlphaPS.cso",
					modelInputDesc, static_cast<UINT>(_countof(modelInputDesc)));

			}
		}

		// カスケードシャドウマップ用
		_cascadedSMShader[static_cast<int>(ModelRenderType::Dynamic)] =
			std::make_unique<CascadedShadowMapShader>(device,
				"./Data/Shader/HLSL/Model/CascadedShadow/CascadedShadowVS.cso",
				modelInputDesc, static_cast<UINT>(_countof(modelInputDesc)));
		_cascadedSMShader[static_cast<int>(ModelRenderType::Static)] =
			std::make_unique<CascadedShadowMapShader>(device,
				"./Data/Shader/HLSL/Model/CascadedShadow/CascadedShadowBatchingVS.cso",
				modelInputDesc, static_cast<UINT>(_countof(modelInputDesc)));
		_cascadedSMShader[static_cast<int>(ModelRenderType::Instancing)] =
			std::make_unique<CascadedShadowMapShader>(device,
				"./Data/Shader/HLSL/Model/CascadedShadow/CascadedShadowInstancedVS.cso",
				modelInputDesc, static_cast<UINT>(_countof(modelInputDesc)));
	}
}

// Gui描画
void ModelShaderResource::DrawGui()
{
}

// マテリアルの編集GUI表示
void ModelShaderResource::DrawMaterialEditGui(Material* material, ModelRenderType type, bool writeGBuffer)
{
	bool isDeferred = writeGBuffer && material->GetBlendType() != BlendType::Alpha;

	// 使用可能なシェーダー取得
	auto activeShaderTypes = GetShaderNames(type, isDeferred);
	// シェーダー変更GUI
	if (ImGui::TreeNode(u8"シェーダー変更"))
	{
		const auto& shaderType = material->GetShaderName();
		for (auto& activeShaderType : activeShaderTypes)
		{
			bool active = activeShaderType == shaderType;
			if (ImGui::RadioButton(activeShaderType, active))
			{
				material->SetShaderName(activeShaderType);
				// シェーダー変更時はパラメータも初期化
				material->SetParameterMap(GetShaderParameterKey(
					type,
					activeShaderType,
					isDeferred));
			}
		}
		ImGui::TreePop();
	}
}

// インスタンシングモデルのマテリアルの編集GUI表示
void ModelShaderResource::DrawInstancingMaterialEditGui(Material* material)
{
	// 使用可能なシェーダー取得
	auto activeShaderTypes = GetInstancingShaderNames();
	// シェーダー変更GUI
	if (ImGui::TreeNode(u8"シェーダー変更"))
	{
		auto shaderType = material->GetShaderName();
		for (auto& activeShaderType : activeShaderTypes)
		{
			bool active = activeShaderType == shaderType;
			if (ImGui::RadioButton(activeShaderType, active))
			{
				material->SetShaderName(activeShaderType);
				// シェーダー変更時はパラメータも初期化
				material->SetParameterMap(GetShaderParameterKey(
					ModelRenderType::Instancing,
					activeShaderType,
					false));
			}
		}
		ImGui::TreePop();
	}
}

// typeとkeyからパラメータのkeyを取得
Material::ParameterMap ModelShaderResource::GetShaderParameterKey(ModelRenderType type, std::string key, bool deferred)
{
	if (deferred)
	{
		return GetShaderMap(true)[static_cast<int>(type)][key]->GetParameterMap();
	}
	else
	{
		return GetShaderMap(false)[static_cast<int>(type)][key]->GetParameterMap();
	}
}
