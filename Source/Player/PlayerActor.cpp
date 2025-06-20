#include "PlayerActor.h"

#include "../../Scene/Scene.h"
#include "../../Library/Component/ModelRenderer.h"
#include "../../Library/Component/Animator.h"
#include "../../Library/Component/CharactorController.h"
#include "../../Library/Component/Effekseer/EffekseerEffectController.h"

#include "../../Source/Common/Targetable.h"
#include "PlayerInput.h"
#include "UI/PlayerHealthUIController.h"

#include "Weapon/Warrior/PlayerShieldActor.h"
#include "Weapon/Warrior/PlayerSwordActor.h"

#include "../../Source/Camera/PlayerCameraController.h"

// 生成時処理
void PlayerActor::OnCreate()
{
	// モデル読み込み
	//auto model = LoadModel("./Data/Model/Player/2025_03_25.fbx");
	auto model = LoadModel(GetModelFilePath());

	// コンポーネント追加
	//auto modelRenderer		= AddComponent<ModelRenderer>("./Data/Model/Player/2025_03_25.fbx");
	auto damageable				= this->AddComponent<Damageable>();
	auto targetable				= this->AddComponent<Targetable>();
	auto modelRenderer			= this->AddComponent<ModelRenderer>();
	auto animator				= this->AddComponent<Animator>();
	auto charactorController	= this->AddComponent<CharactorController>();
	auto playerController		= this->AddComponent<PlayerController>();
	auto effekseerController	= this->AddComponent<EffekseerEffectController>("./Data/Effect/Effekseer/Player/Attack_Impact.efk");
	auto hpUIController			= this->AddComponent<PlayerHealthUIController>(_isUserControlled, damageable);
	// プレイヤーが操作する場合は、プレイヤーコントローラーを追加
	if (_isUserControlled)
		this->AddComponent<PlayerInput>();

	// コライダー設定
	auto collider = this->AddCollider<CapsuleCollider>();

	// パラメータ設定
	GetTransform().SetLengthScale(0.01f);
	damageable->SetMaxHealth(10.0f);
	targetable->SetFaction(Targetable::Faction::Player);
	// 操作対象でなければ攻撃力の倍率を0にしてダメージを与えられないようにする
	if (!_isUserControlled)
		playerController->SetATKFactor(0.0f);

	collider->SetStart(Vector3(0.0f, 50.0f, 0.0f));
	collider->SetEnd(Vector3(0.0f, 130.0f, 0.0f));
	collider->SetRadius(0.5f);
	collider->SetLayer(CollisionLayer::Hit);

	// 剣生成
	{
		auto sword = this->_scene->RegisterActor<PlayerSwordActor>(GetName() + std::string(u8"Sword"), ActorTag::Player);
		//const ModelResource::Node* rightHandNode = &(model.lock()->GetPoseNodes().at(model.lock()->GetNodeIndex("RightHand")));
		const ModelResource::Node* rightHandNode = &(model.lock()->GetPoseNodes().at(model.lock()->GetNodeIndex("ORG-hand.R")));
		sword->Initialize(this, rightHandNode);
		_swordActor = sword;
	}

	// 盾生成
	{
		auto shield = this->_scene->RegisterActor<PlayerShieldActor>(GetName() + std::string(u8"Shield"), ActorTag::Player);
		//const ModelResource::Node* leftForeArmNode = &(model.lock()->GetPoseNodes().at(model.lock()->GetNodeIndex("LeftForeArm")));
		const ModelResource::Node* leftForeArmNode = &(model.lock()->GetPoseNodes().at(model.lock()->GetNodeIndex("ORG-hand.L")));
		shield->Initialize(this, leftForeArmNode);
		_shieldActor = shield;
	}

	// カメラ作成
	if (_isUserControlled)
	{
		auto camera = this->_scene->RegisterActor<Actor>(u8"PlayerCamera", ActorTag::DrawContextParameter);
        camera->AddComponent<PlayerCameraController>(this);
	}
}

// 削除処理
void PlayerActor::OnDeleted()
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

