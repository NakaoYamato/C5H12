#include "PlayerActor.h"

#include "../../Scene/Scene.h"
#include "../../Library/Component/ModelRenderer.h"
#include "../../Library/Component/Animator.h"
#include "../../Library/Component/CharactorController.h"
#include "../../Library/Component/Effekseer/EffekseerEffectController.h"
#include "PlayerInput.h"

#include "../../Source/Camera/PlayerCameraController.h"

#include "Weapon/Warrior/PlayerShieldActor.h"
#include "Weapon/Warrior/PlayerSwordActor.h"

// 生成時処理
void PlayerActor::OnCreate()
{
	// モデル読み込み
	//auto model = LoadModel("./Data/Model/Player/2025_03_25.fbx");
	auto model = LoadModel("./Data/Model/Player/ARPG_Warrior.fbx");
	GetTransform().SetLengthScale(0.01f);

	// コンポーネント追加
	//auto modelRenderer = AddComponent<ModelRenderer>("./Data/Model/Player/2025_03_25.fbx");
	_damageable			= this->AddComponent<Damageable>();
	auto modelRenderer		= this->AddComponent<ModelRenderer>();
	auto animator			= this->AddComponent<Animator>();
	_charactorController	= this->AddComponent<CharactorController>();
	_playerController		= this->AddComponent<PlayerController>();
	auto effekseerController = this->AddComponent<EffekseerEffectController>("./Data/Effect/Effekseer/Player/Attack_Impact.efk");
	// プレイヤーが操作する場合は、プレイヤーコントローラーを追加
	if (_isUserControlled)
	{
		this->AddComponent<PlayerInput>();
	}
	else
	{
		// 攻撃力の倍率を0にしてダメージを与えられないようにする
		_playerController.lock()->SetATKFactor(0.0f);
	}

	// コライダー設定
    auto collider		= this->AddCollider<CapsuleCollider>();
	collider->SetStart(Vector3(0.0f, 50.0f, 0.0f));
	collider->SetEnd(Vector3(0.0f, 130.0f, 0.0f));
	collider->SetRadius(0.5f);

	// 剣生成
	auto sword = this->_scene->RegisterActor<PlayerSwordActor>(GetName() + std::string(u8"Sword"), ActorTag::Player);
	//const ModelResource::Node* rightHandNode = &(model.lock()->GetPoseNodes().at(model.lock()->GetNodeIndex("RightHand")));
	const ModelResource::Node* rightHandNode = &(model.lock()->GetPoseNodes().at(model.lock()->GetNodeIndex("ORG-hand.R")));
	sword->Initialize(this, rightHandNode);
    _swordActor = sword;

	// 盾生成
	auto shield = this->_scene->RegisterActor<PlayerShieldActor>(GetName() + std::string(u8"Shield"), ActorTag::Player);
	//const ModelResource::Node* leftForeArmNode = &(model.lock()->GetPoseNodes().at(model.lock()->GetNodeIndex("LeftForeArm")));
	const ModelResource::Node* leftForeArmNode = &(model.lock()->GetPoseNodes().at(model.lock()->GetNodeIndex("ORG-hand.L")));
	shield->Initialize(this, leftForeArmNode);
    _shieldActor = shield;

	if (_isUserControlled)
	{
		// カメラ作成
		auto camera = this->_scene->RegisterActor<Actor>(u8"PlayerCamera", ActorTag::DrawContextParameter);
        camera->AddComponent<PlayerCameraController>(this);
	}
}

// 削除処理
void PlayerActor::OnRemove()
{
	if (_swordActor.lock())
	{
		// 剣削除
        _swordActor.lock()->Remove();
	}
	if (_shieldActor.lock())
	{
		// 盾削除
		_shieldActor.lock()->Remove();
	}
}

