#include "PlayerActor.h"

#include "../../Scene/Scene.h"
#include "../../Library/Component/ModelRenderer.h"
#include "../../Library/Component/Animator.h"
#include "../../Library/Component/CharactorController.h"
#include "../../Source/Component/Player/PlayerInput.h"
#include "../../Source/Component/Player/PlayerController.h"

#include "PlayerSwordActor.h"
#include "PlayerShieldActor.h"

// 生成時処理
void PlayerActor::OnCreate()
{
	GetTransform().SetLengthScale(0.01f);
	// コンポーネント追加
	auto modelRenderer = AddComponent<ModelRenderer>("./Data/Model/Player/2025_03_25.fbx");
	auto animator = AddComponent<Animator>(modelRenderer->GetModel());
	auto charactorController = AddComponent<CharactorController>();
	auto playerInput = AddComponent<PlayerInput>();
	auto playerController = AddComponent<PlayerController>(_isUserControlled);

	// 剣生成
	auto sword = this->_scene->RegisterActor<PlayerSwordActor>(GetName() + std::string(u8"Sword"), ActorTag::Player);
	const ModelResource::Node* rightHandNode = &(modelRenderer->GetModel()->GetPoseNodes().at(modelRenderer->GetModel()->GetNodeIndex("RightHand")));
	sword->Initialize(this, rightHandNode);
    _swordActor = sword;

	// 盾生成
	auto shield = this->_scene->RegisterActor<PlayerShieldActor>(GetName() + std::string(u8"Shield"), ActorTag::Player);
	const ModelResource::Node* leftForeArmNode = &(modelRenderer->GetModel()->GetPoseNodes().at(modelRenderer->GetModel()->GetNodeIndex("LeftForeArm")));
	shield->Initialize(this, leftForeArmNode);
    _shieldActor = shield;
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

