#include "PlayerActor.h"

#include "../../Scene/Scene.h"
#include "../../Library/Component/ModelRenderer.h"
#include "../../Library/Component/Animator.h"
#include "../../Library/Component/CharactorController.h"
#include "../../Source/Component/Player/PlayerInput.h"

#include "../../Source/Component/Camera/PlayerCameraController.h"

#include "PlayerSwordActor.h"
#include "PlayerShieldActor.h"

// 生成時処理
void PlayerActor::OnCreate()
{
	Actor::OnCreate();

	// モデル読み込み
	//auto model = LoadModel("./Data/Model/Player/2025_03_25.fbx");
	auto model = LoadModel("./Data/Model/Player/ARPG_Warrior.fbx");
	GetTransform().SetLengthScale(0.01f);

	// コンポーネント追加
	//auto modelRenderer = AddComponent<ModelRenderer>("./Data/Model/Player/2025_03_25.fbx");
	auto modelRenderer = AddComponent<ModelRenderer>();
	auto animator = AddComponent<Animator>();
	_charactorController = AddComponent<CharactorController>();
	auto playerInput = AddComponent<PlayerInput>();
	_playerController = AddComponent<PlayerController>(_isUserControlled);

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
void PlayerActor::Destroy()
{
	Actor::Destroy();

	if (_swordActor.lock())
	{
		// 剣削除
        _swordActor.lock()->Destroy();
	}
	if (_shieldActor.lock())
	{
		// 盾削除
		_shieldActor.lock()->Destroy();
	}
}

