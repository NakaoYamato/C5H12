#include "PlayerActor.h"

#include "../../Scene/Scene.h"
#include "../../Library/Component/ModelRenderer.h"
#include "../../Library/Component/Animator.h"
#include "../../Library/Component/AnimatorCamera.h"
#include "../../Library/Component/CharactorController.h"
#include "../../Library/Component/EffectController.h"
#include "../../Library/Component/Collider/ModelCollider.h"
#include "../../Library/Shader/Model/ModelShaderResource.h"

#include "../../Source/Common/Targetable.h"
#include "../../Source/Common/DamageSender.h"
#include "../../Source/Common/StaminaController.h"
#include "../../Source/Armor/ArmorActor.h"
#include "../../Source/Camera/PlayerCameraController.h"
#include "../../Source/Camera/PlayerDeathCamera.h"
#include "../../Source/Camera/LockOnCamera.h"
#include "../../Source/InGame/InGameCanvasActor.h"
#include "PlayerItemController.h"
#include "PlayerInput.h"
#include "PlayerEquipmentController.h"
#include "BuffController.h"
#include "StateMachine/PlayerStateController.h"

#include "Weapon/GreatSword/PlayerGreatSwordActor.h"

#include "../../Source/Network/NetworkReceiver.h"
#include "PlayerNetworkSender.h"


// 生成時処理
void PlayerActor::OnCreate()
{
	// モデル読み込み
	//auto model = LoadModel("./Data/Model/Player/2025_03_25.fbx");
	auto model = LoadModel(GetModelFilePath());

	// コンポーネント追加
	//auto modelRenderer		= AddComponent<ModelRenderer>("./Data/Model/Player/2025_03_25.fbx");
	auto damageable				= this->AddComponent<Damageable>();
	auto damageSender			= this->AddComponent<DamageSender>();
    auto staminaController		= this->AddComponent<StaminaController>();
	auto targetable				= this->AddComponent<Targetable>();
	auto modelRenderer			= this->AddComponent<ModelRenderer>();
	auto animator				= this->AddComponent<Animator>();
	auto animatorCamera			= this->AddComponent<AnimatorCamera>();
	auto charactorController	= this->AddComponent<CharactorController>();
	auto playerController		= this->AddComponent<PlayerController>();
	auto equipmentController	= this->AddComponent<PlayerEquipmentController>();
	auto itemController			= this->AddComponent<PlayerItemController>();
	auto buffController			= this->AddComponent<BuffController>();
	auto effectController		= this->AddComponent<EffectController>();
	auto playerStateController	= this->AddComponent<PlayerStateController>();
	auto networkReceiver		= this->AddComponent<NetworkReceiver>();
	auto networkSender			= this->AddComponent<PlayerNetworkSender>();

	// エフェクト読み込み
	{
		effectController->LoadEffekseerEffect(PlayerController::EffectType::HitEffect, "./Data/Effect/Effekseer/Player/Attack_Impact.efk");
		effectController->LoadEffekseerEffect(PlayerController::EffectType::Charge0, "./Data/Effect/Effekseer/Player/Charge.efk");
		{
			EffectController::EffekseerEffectData* effectData =
				((EffectController::EffekseerEffectData*)effectController->GetEffectData(PlayerController::EffectType::Charge0));
			effectData->SetAllColor(Vector4::Yellow);
			effectData->SetScale(Vector3(0.3f, 0.3f, 0.3f));
		}
		effectController->LoadEffekseerEffect(PlayerController::EffectType::Charge1, "./Data/Effect/Effekseer/Player/Charge.efk");
		{
			EffectController::EffekseerEffectData* effectData =
				((EffectController::EffekseerEffectData*)effectController->GetEffectData(PlayerController::EffectType::Charge1));
			effectData->SetAllColor(Vector4::Orange);
			effectData->SetScale(Vector3(0.3f, 0.3f, 0.3f));
		}
		effectController->LoadEffekseerEffect(PlayerController::EffectType::Charge2, "./Data/Effect/Effekseer/Player/Charge.efk");
		{
			EffectController::EffekseerEffectData* effectData =
				((EffectController::EffekseerEffectData*)effectController->GetEffectData(PlayerController::EffectType::Charge2));
			effectData->SetAllColor(Vector4::Red);
			effectData->SetScale(Vector3(0.3f, 0.3f, 0.3f));
		}
	}

    // ネットワーク受信イベントの設定
	if (networkReceiver)
	{
		networkReceiver->GetEventBus().Subscribe<Network::CharacterSync>([this](const Network::CharacterSync& sync)
			{
				this->GetTransform().SetPosition(sync.position);
				this->GetTransform().SetAngleY(sync.angleY);
				auto damageable = this->GetComponent<Damageable>();
				if (damageable)
				{
					damageable->ResetHealth(sync.health);
				}
			});
        networkReceiver->GetEventBus().Subscribe<Network::CharacterMove>([this](const Network::CharacterMove& move)
            {
                this->GetTransform().SetPosition(move.position);
                this->GetTransform().SetAngleY(move.angleY);
				auto playerController = this->GetComponent<PlayerController>();
				if (playerController)
				{
					Vector2 movement = Vector2(move.target.x, move.target.z);
					playerController->SetMovement(movement);
				}
     //           auto stateController = this->GetComponent<StateController>();
     //           if (stateController)
     //           { 
					//// プレイヤーの状態を更新
					//// TODO
     //               //auto playerStateMachine = std::dynamic_pointer_cast<PlayerStateMachine>(stateController->GetStateMachine());
     //               //if (playerStateMachine)
     //               //{
     //               //    playerStateMachine->ChangeState(move.mainState, move.subState);
     //               //}
     //           }
            });
		networkReceiver->GetEventBus().Subscribe<Network::CharacterApplyDamage>([this](const Network::CharacterApplyDamage& applyDamage)
			{
				auto damageable = this->GetComponent<Damageable>();
				if (damageable)
				{
					damageable->SetHelth(damageable->GetHealth() - applyDamage.damage);
					damageable->SetHitPosition(applyDamage.hitPosition);
				}
			});
	}

	// コライダー設定
	auto capsuleCollider = this->AddCollider<CapsuleCollider>();
	auto modelCollider = this->AddCollider<ModelCollider>();
    modelCollider->SetLayer(CollisionLayer::Hit);

	// パラメータ設定
	GetTransform().SetLengthScale(1.0f);
	GetTransform().SetPositionY(1.0f);

	auto modelShaderResource = ResourceManager::Instance().GetResourceAs<ModelShaderResource>();
	for (auto& material : modelRenderer->GetMaterials())
	{
		material.SetShaderName("Player");
		// シェーダー変更時はパラメータも初期化
		material.SetParameterMap(modelShaderResource->GetShaderParameterKey(
			modelRenderer->GetRenderType(),
			"Player",
			true));
	}

	damageable->SetMaxHealth(100.0f);
	damageSender->SetDrawText(true);
	damageSender->SetHitEffectIndex(PlayerController::EffectType::HitEffect);
    staminaController->SetStaminaRecoverSpeed(10.0f);

	targetable->SetFaction(Targetable::Faction::Player);

	capsuleCollider->SetStart(Vector3(0.0f, 0.0f, 0.0f));
	capsuleCollider->SetEnd(Vector3(0.0f, 1.2f, 0.0f));
	capsuleCollider->SetRadius(0.5f);
	capsuleCollider->SetLayer(CollisionLayer::Hit);

	// 剣生成
	{
		auto sword = this->_scene->RegisterActor<PlayerGreatSwordActor>(GetName() + std::string(u8"GreatSword"), ActorTag::Player);
		const ModelResource::Node* rightHandNode = &(model.lock()->GetPoseNodes().at(model.lock()->GetNodeIndex("weapon_r")));
		sword->Initialize(this, rightHandNode);
		_swordActor = sword;
		equipmentController->SetWeaponActor(sword);
	}

	// 防具生成
	{
		auto head = this->_scene->RegisterActor<ArmorActor>(
			GetName() + std::string(u8"Head"), 
			ActorTag::Player,
			ArmorType::Head,
			equipmentController->GetArmorIndex(ArmorType::Head));
		head->SetParent(this);
		equipmentController->SetArmorActor(ArmorType::Head, head);
	}
	{
		auto chest = this->_scene->RegisterActor<ArmorActor>(
			GetName() + std::string(u8"Chest"), 
			ActorTag::Player, 
			ArmorType::Chest,
			equipmentController->GetArmorIndex(ArmorType::Chest));
		chest->SetParent(this);
		equipmentController->SetArmorActor(ArmorType::Chest, chest);
	}
	{
		auto arm = this->_scene->RegisterActor<ArmorActor>(
			GetName() + std::string(u8"Arm"),
			ActorTag::Player,
			ArmorType::Arm,
			equipmentController->GetArmorIndex(ArmorType::Arm));
		arm->SetParent(this);
		equipmentController->SetArmorActor(ArmorType::Arm, arm);
	}
	{
		auto Waist = this->_scene->RegisterActor<ArmorActor>(
			GetName() + std::string(u8"Waist"),
			ActorTag::Player,
			ArmorType::Waist,
			equipmentController->GetArmorIndex(ArmorType::Waist));
		Waist->SetParent(this);
		equipmentController->SetArmorActor(ArmorType::Waist, Waist);
	}
	{
		auto Leg = this->_scene->RegisterActor<ArmorActor>(
			GetName() + std::string(u8"Leg"), 
			ActorTag::Player,
			ArmorType::Leg,
			equipmentController->GetArmorIndex(ArmorType::Leg));
		Leg->SetParent(this);
		equipmentController->SetArmorActor(ArmorType::Leg, Leg);
	}

	// ユーザー操作プレイヤーの場合
	if (_isUserControlled)
	{
		// プレイヤーインプット追加
		auto input = this->AddComponent<PlayerInput>();
		input->Swich();
	}
	else
	{
		// 操作対象でなければ攻撃力の倍率を0にしてダメージを与えられないようにする
		damageSender->SetBaseATK(0.0f);
	}
}

// 開始時処理
void PlayerActor::OnStart()
{
	// ユーザー操作プレイヤーの場合
	if (_isUserControlled)
	{
		// UI設定
		auto inGameCanvas = this->GetScene()->GetActorManager().FindByClass<InGameCanvasActor>(ActorTag::UI);
		if (inGameCanvas)
		{
			inGameCanvas->RegisterPlayerActor(this);
		}

		// カメラ設定
		auto playerCamera = GetScene()->GetMainCameraActor()->GetControllerByClass<PlayerCameraController>();
		if (playerCamera)
		{
			playerCamera->SetPlayerActor(this);
			playerCamera->Swich();
		}
		auto lockOnCamera = GetScene()->GetMainCameraActor()->GetControllerByClass<LockOnCamera>();
		if (lockOnCamera)
		{
			lockOnCamera->SetPlayerActor(this);
		}
		auto deathCamera = GetScene()->GetMainCameraActor()->GetControllerByClass<PlayerDeathCamera>();
		if (deathCamera)
		{
			deathCamera->SetPlayerActor(this);
		}
	}
}
