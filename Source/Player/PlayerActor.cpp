#include "PlayerActor.h"

#include "../../Scene/Scene.h"
#include "../../Library/Component/ModelRenderer.h"
#include "../../Library/Component/Animator.h"
#include "../../Library/Component/AnimatorCamera.h"
#include "../../Library/Component/CharactorController.h"
#include "../../Library/Component/EffectController.h"
#include "../../Library/Component/Collider/ModelCollider.h"

#include "../../Source/Common/Targetable.h"
#include "../../Source/Common/DamageSender.h"
#include "../../Source/Common/StaminaController.h"
#include "PlayerInput.h"
#include "StateMachine/PlayerStateMachine.h"
#include "UI/PlayerHealthUIController.h"
#include "UI/PlayerStaminaUIController.h"

#include "Weapon/Warrior/PlayerShieldActor.h"
#include "Weapon/Warrior/PlayerSwordActor.h"
#include "Weapon/GreatSword/PlayerGreatSwordActor.h"

#include "../../Source/Camera/PlayerCameraController.h"

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
    auto staminaController = this->AddComponent<StaminaController>();
	auto targetable				= this->AddComponent<Targetable>();
	auto modelRenderer			= this->AddComponent<ModelRenderer>();
	auto animator				= this->AddComponent<Animator>();
	auto animatorCamera			= this->AddComponent<AnimatorCamera>();
	auto charactorController	= this->AddComponent<CharactorController>();
	auto playerController		= this->AddComponent<PlayerController>();
	auto effectController		= this->AddComponent<EffectController>();
	auto stateController		= this->AddComponent<StateController>(std::make_shared<PlayerStateMachine>(this));
	auto networkReceiver		= this->AddComponent<NetworkReceiver>();
	auto networkSender			= this->AddComponent<PlayerNetworkSender>();

	// UI生成
	{
        auto hpUIActor = this->_scene->RegisterActor<UIActor>(GetName() + std::string(u8"HPUI"), ActorTag::UI);
		hpUIActor->GetRectTransform().SetLocalPosition(Vector2(50.0f, 50.0f));
		hpUIActor->GetRectTransform().SetLocalScale(Vector2(1.0f, 0.5f));
		auto hpUIController = hpUIActor->AddComponent<PlayerHealthUIController>(_isUserControlled, damageable);

		if (_isUserControlled)
		{
			auto staminaUIActor = this->_scene->RegisterActor<UIActor>(GetName() + std::string(u8"StaminaUI"), ActorTag::UI);
			staminaUIActor->GetRectTransform().SetLocalPosition(Vector2(50.0f, 74.0f));
			staminaUIActor->GetRectTransform().SetLocalScale(Vector2(1.0f, 0.5f));
			auto staminaUIController = staminaUIActor->AddComponent<PlayerStaminaUIController>(staminaController);
		}
	}

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

	// プレイヤーが操作する場合は、プレイヤーインプットを追加
	if (_isUserControlled)
		this->AddComponent<PlayerInput>();

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
                auto stateController = this->GetComponent<StateController>();
                if (stateController)
                { 
					// プレイヤーの状態を更新
                    auto playerStateMachine = std::dynamic_pointer_cast<PlayerStateMachine>(stateController->GetStateMachine());
                    if (playerStateMachine)
                    {
                        playerStateMachine->ChangeState(move.mainState, move.subState);
                    }
                }
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

	// パラメータ設定
	GetTransform().SetLengthScale(1.0f);
	GetTransform().SetPositionY(1.0f);

	for (auto& material : modelRenderer->GetMaterials())
	{
		material.SetShaderName("Player");
		// シェーダー変更時はパラメータも初期化
		material.SetParameterMap(GetScene()->GetMeshRenderer().GetShaderParameterKey(
			modelRenderer->GetRenderType(),
			"Player",
			true));
	}

	damageable->SetMaxHealth(10.0f);
	damageSender->SetHitEffectIndex(PlayerController::EffectType::HitEffect);
    staminaController->SetStaminaRecoverSpeed(10.0f);
	// 操作対象でなければ攻撃力の倍率を0にしてダメージを与えられないようにする
	if (!_isUserControlled)
		damageSender->SetBaseATK(0.0f);

	targetable->SetFaction(Targetable::Faction::Player);

	capsuleCollider->SetStart(Vector3(0.0f, 0.0f, 0.0f));
	capsuleCollider->SetEnd(Vector3(0.0f, 1.2f, 0.0f));
	capsuleCollider->SetRadius(0.5f);
	capsuleCollider->SetLayer(CollisionLayer::Hit);

	//// 剣生成
	//{
	//	auto sword = this->_scene->RegisterActor<PlayerSwordActor>(GetName() + std::string(u8"Sword"), ActorTag::Player);
	//	//const ModelResource::Node* rightHandNode = &(model.lock()->GetPoseNodes().at(model.lock()->GetNodeIndex("RightHand")));
	//	const ModelResource::Node* rightHandNode = &(model.lock()->GetPoseNodes().at(model.lock()->GetNodeIndex("ORG-hand.R")));
	//	sword->Initialize(this, rightHandNode);
	//	_swordActor = sword;
	//}

	//// 盾生成
	//{
	//	auto shield = this->_scene->RegisterActor<PlayerShieldActor>(GetName() + std::string(u8"Shield"), ActorTag::Player);
	//	//const ModelResource::Node* leftForeArmNode = &(model.lock()->GetPoseNodes().at(model.lock()->GetNodeIndex("LeftForeArm")));
	//	const ModelResource::Node* leftForeArmNode = &(model.lock()->GetPoseNodes().at(model.lock()->GetNodeIndex("ORG-hand.L")));
	//	shield->Initialize(this, leftForeArmNode);
	//	_shieldActor = shield;
	//}
	// 剣生成
	{
		auto sword = this->_scene->RegisterActor<PlayerGreatSwordActor>(GetName() + std::string(u8"GreatSword"), ActorTag::Player);
		const ModelResource::Node* rightHandNode = &(model.lock()->GetPoseNodes().at(model.lock()->GetNodeIndex("weapon_r")));
		sword->Initialize(this, rightHandNode);
		_swordActor = sword;
	}

	// カメラ作成
	if (_isUserControlled)
	{
		auto camera = this->_scene->RegisterActor<Actor>(u8"PlayerCamera", ActorTag::DrawContextParameter);
        camera->AddComponent<PlayerCameraController>(this);
	}
}
