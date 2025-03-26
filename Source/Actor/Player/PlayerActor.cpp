#include "PlayerActor.h"

#include "../../Scene/Scene.h"
#include "../../Library/Component/ModelRenderer.h"
#include "../../Library/Component/Animator.h"
#include "PlayerSwordActor.h"
#include "PlayerShieldActor.h"

// 生成時処理
void PlayerActor::OnCreate()
{
	GetTransform().SetLengthScale(0.01f);
	// コンポーネント追加
	auto modelRenderer = AddComponent<ModelRenderer>("./Data/Model/Player/2025_03_25.fbx");
	auto animator = AddComponent<Animator>(modelRenderer->GetModel());

	// 剣生成
	auto sword = this->_scene->RegisterActor<PlayerSwordActor>(u8"PlayerSword", ActorTag::Player);
	const ModelResource::Node* rightHandNode = &(modelRenderer->GetModel()->GetPoseNodes().at(modelRenderer->GetModel()->GetNodeIndex("RightHand")));
	sword->Initialize(this, rightHandNode);

	// 盾生成
	auto shield = this->_scene->RegisterActor<PlayerShieldActor>(u8"PlayerShield", ActorTag::Player);
	const ModelResource::Node* leftForeArmNode = &(modelRenderer->GetModel()->GetPoseNodes().at(modelRenderer->GetModel()->GetNodeIndex("LeftForeArm")));
	shield->Initialize(this, leftForeArmNode);
}
