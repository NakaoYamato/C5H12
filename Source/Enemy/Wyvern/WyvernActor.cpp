#include "WyvernActor.h"

#include "../../Library/Scene/Scene.h"
#include "../../Library/Graphics/Graphics.h"
#include "../../Library/Graphics/GpuResourceManager.h"
#include "../../Library/Algorithm/Converter.h"
#include "../../Library/Component/Collider/ModelCollider.h"
#include "../../Library/Component/Effekseer/EffekseerEffectController.h"

#include "../EnemyController.h"
#include "WyvernController.h"
#include "StateMachine/WyvernStateMachine.h"
#include "BehaviorTree/WyvernBehaviorTree.h"
#include "../../Source/Player/PlayerController.h"

#include <imgui.h>

// 生成時処理
void WyvernActor::OnCreate()
{
	EnemyActor::OnCreate();

	// モデル読み込み
	auto model = LoadModel(GetModelFilePath());
	// モデルがシリアライズされていなければエラー
	assert(model.lock()->GetResource()->IsSerialized());

	GetTransform().SetLengthScale(0.01f);
	GetTransform().SetScale(1.5f);
	_charactorController.lock()->SetMass(1000.0f);

	// コンポーネント追加
	_modelRenderer				= AddComponent<ModelRenderer>();
	_animator					= AddComponent<Animator>();
	auto enemyController		= AddComponent<EnemyController>();
	auto wyvernController		= AddComponent<WyvernController>();
	auto stateMachine = std::make_shared<WyvernStateMachine>(
		enemyController.get(),
		wyvernController.get(),
		_animator.lock().get(),
		_damageable.lock().get());
	auto stateController		= AddComponent<StateController>(stateMachine);
	auto behaviorController		= AddComponent<BehaviorController>(std::make_shared<WyvernBehaviorTree>(stateMachine.get(), _animator.lock().get()));
	auto effekseerController = this->AddComponent<EffekseerEffectController>("./Data/Effect/Effekseer/Player/Attack_Impact.efk");

	// コライダー追加
	auto modelCollider = AddCollider<ModelCollider>();
	modelCollider->SetLayer(CollisionLayer::Hit);
}
// 更新処理
void WyvernActor::OnUpdate(float elapsedTime)
{
	// 使用する角を設定
	SetUseHorn();
}
// GUI描画
void WyvernActor::OnDrawGui()
{
	static const char* modelTypeName[] =
	{
		u8"Brown",
		u8"Green",
		u8"Lava",
		u8"Red",
		u8"White",
	};
	static const char* drawHornTypeName[] =
	{
		u8"Horn01",
		u8"Horn02",
		u8"Horn03",
		u8"Horn04",
		u8"Horn05",
		u8"Horn06",
	};
	static ImGuiTabBarFlags tab_bar_flags =
		ImGuiTabBarFlags_AutoSelectNewTabs |
		ImGuiTabBarFlags_Reorderable |
		ImGuiTabBarFlags_FittingPolicyResizeDown;


	if (ImGui::BeginTabBar("MyTabBar", tab_bar_flags))
	{
		if (ImGui::BeginTabItem(u8"ワイバーン"))
		{
			int mt = static_cast<int>(_textureType);
			if (ImGui::Combo(u8"使用するテクスチャ", &mt, modelTypeName, _countof(modelTypeName)))
			{
				_textureType = static_cast<TextureType>(mt);
				SetModelTexture();
				// 再シリアライズ
				GetModel().lock()->ReSerialize();
			}
			int dht = static_cast<int>(_drawHornType);
			if (ImGui::Combo(u8"使用する角", &dht, drawHornTypeName, _countof(drawHornTypeName)))
			{
				_drawHornType = static_cast<DrawHornType>(dht);
				SetUseHorn();
			}

			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}
}

// モデルのテクスチャを設定
void WyvernActor::SetModelTexture()
{
	auto resource = GetModel().lock()->GetResource();
	auto SetModelSRV = [&](const wchar_t* filename, std::string materialName, std::string textureKey)
		{
			static std::wstring RelativePath = L"./Data/Model/Dragons/";

			// テクスチャからSRV作成
			Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv;
			GpuResourceManager::LoadTextureFromFile(Graphics::Instance().GetDevice(),
				std::wstring(RelativePath + filename).c_str(),
				srv.ReleaseAndGetAddressOf(),
				nullptr);

			// SRVの設定
			_modelRenderer.lock()->ChangeMaterialSRV(srv, materialName, textureKey);

			// テクスチャのファイル設定
			for (auto& material : resource->GetAddressMaterials())
			{
				if (material.name == materialName)
				{
					material.textureDatas.at(textureKey).filename = ToString(filename).c_str();
					return;
				}
			}
		};

	// 共通部分のテクスチャ
#pragma region Body
	SetModelSRV(L"Texture/T_Kuzar_Body_Normal.png", "Material #25", "Normal");
	SetModelSRV(L"Texture/T_Kuzar_Body_EmissionBW.png", "Material #25", "Emissive");
#pragma endregion
#pragma region Spikes
	SetModelSRV(L"Texture/T_Kuzar_Spikes_Albedo.png", "Material #26", "Diffuse");
	SetModelSRV(L"Texture/T_Kuzar_Spikes_Normal.png", "Material #26", "Normal");
#pragma endregion
#pragma region Wings
	SetModelSRV(L"Texture/T_Kuzar_Wings_Normal.png", "Material #31", "Normal");
	SetModelSRV(L"Texture/T_Kuzar_Wings_EmissionBW.png", "Material #31", "Emissive");
#pragma endregion
#pragma region Horns
	SetModelSRV(L"Texture/T_Kuzar_Horns_Color.png", "Horns", "Diffuse");
	SetModelSRV(L"Texture/T_Kuzar_Horns_Normal.png", "Horns", "Normal");
	SetModelSRV(L"Texture/T_Kuzar_Horns_Emissive.png", "Horns", "Emissive");
#pragma endregion
#pragma region Eyes
	SetModelSRV(L"Texture/T_Dragon_Eyes_Albedo.png", "Eyes", "Diffuse");
	SetModelSRV(L"Texture/T_Dragon_Eyes_Normal.png", "Eyes", "Normal");
#pragma endregion

	switch (_textureType)
	{
	case WyvernActor::TextureType::Brown:
#pragma region Body
		SetModelSRV(L"Texture/Brown/T_Kuzar_Brown_Body_Albedo.png", "Material #25", "Diffuse");
		SetModelSRV(L"Texture/Brown/T_Kuzar_Brown_Body_Rough_M_AO.png", "Material #25", "Roughness");
#pragma region Wings
#pragma endregion
		SetModelSRV(L"Texture/Brown/T_Kuzar_Brown_Wings_Albedo.png", "Material #31", "Diffuse");
		SetModelSRV(L"Texture/Brown/T_Kuzar_Brown_Wings_Rough_M_AO.png", "Material #31", "Roughness");
#pragma endregion
		break;
	case WyvernActor::TextureType::Green:
#pragma region Body
		SetModelSRV(L"Texture/Green/T_Kuzar_Green_Body_Albedo.png", "Material #25", "Diffuse");
		SetModelSRV(L"Texture/Green/T_Kuzar_Green_Body_Rough_M_AO.png", "Material #25", "Roughness");
#pragma region Wings
#pragma endregion
		SetModelSRV(L"Texture/Green/T_Kuzar_Green_Wings_Albedo.png", "Material #31", "Diffuse");
		SetModelSRV(L"Texture/Green/T_Kuzar_Green_Wings_Rough_M_AO.png", "Material #31", "Roughness");
#pragma endregion
		break;
	case WyvernActor::TextureType::Lava:
#pragma region Body
		SetModelSRV(L"Texture/Lava/T_Kuzar_Lava_Body_Albedo.png", "Material #25", "Diffuse");
		SetModelSRV(L"Texture/Lava/T_Kuzar_Lava_Body_Rough_M_AO.png", "Material #25", "Roughness");
		SetModelSRV(L"Texture/Lava/T_Kuzar_Lava_Body_Emissive.png", "Material #25", "Emissive");
#pragma region Wings
#pragma endregion
		SetModelSRV(L"Texture/Lava/T_Kuzar_Lava_Wings_Albedo.png", "Material #31", "Diffuse");
		SetModelSRV(L"Texture/Lava/T_Kuzar_Lava_Wings_Rough_M_AO.png", "Material #31", "Roughness");
		SetModelSRV(L"Texture/Lava/T_Kuzar_Lava_Wings_Emissive.png", "Material #31", "Emissive");
#pragma endregion
		break;
	case WyvernActor::TextureType::Red:
#pragma region Body
		SetModelSRV(L"Texture/Red/T_Kuzar_Red_Body_Albedo.png", "Material #25", "Diffuse");
		SetModelSRV(L"Texture/Red/T_Kuzar_Red_Body_Rough_M_AO.png", "Material #25", "Roughness");
#pragma region Wings
#pragma endregion
		SetModelSRV(L"Texture/Red/T_Kuzar_Red_Wings_Albedo.png", "Material #31", "Diffuse");
		SetModelSRV(L"Texture/Red/T_Kuzar_Red_Wings_Rough_M_AO.png", "Material #31", "Roughness");
#pragma endregion
		break;
	case WyvernActor::TextureType::White:
#pragma region Body
		SetModelSRV(L"Texture/White/T_Kuzar_White_Body_Albedo.png", "Material #25", "Diffuse");
		SetModelSRV(L"Texture/White/T_Kuzar_White_Body_Rough_M_AO.png", "Material #25", "Roughness");
#pragma region Wings
#pragma endregion
		SetModelSRV(L"Texture/White/T_Kuzar_White_Wings_Albedo.png", "Material #31", "Diffuse");
		SetModelSRV(L"Texture/White/T_Kuzar_White_Wings_Rough_M_AO.png", "Material #31", "Roughness");
#pragma endregion
		break;
	}
}

// 使用する角を設定
void WyvernActor::SetUseHorn()
{
	auto& nodes = GetModel().lock()->GetPoseNodes();
	// posenodeから"Kuzar3_Horn"を含むnodeを全取得
	ModelResource::Node* horns[6] = {};
	std::vector<int> hornIncludeIndices;
	int index = 0;
	for (auto& node : nodes)
	{
		if (node.name.find("Kuzar3_Horn") != std::string::npos)
			hornIncludeIndices.push_back(index);
		index++;
	}

	static std::string HornNames[] =
	{
		"Kuzar3_Horn01",
		"Kuzar3_Horn02",
		"Kuzar3_Horn03",
		"Kuzar3_Horn04",
		"Kuzar3_Horn05",
		"Kuzar3_Horn06",
	};
	for (int i : hornIncludeIndices)
	{
		if (nodes[i].name == HornNames[static_cast<int>(_drawHornType)])
			nodes[i].scale = Vector3::One;
		else
			nodes[i].scale = Vector3::Zero;
	}
}
