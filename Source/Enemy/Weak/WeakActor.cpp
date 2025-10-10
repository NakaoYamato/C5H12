#include "WeakActor.h"

#include "../../Library/Scene/Scene.h"
#include "../../Library/Component/Collider/ModelCollider.h"
#include "../../Library/Component/ModelRenderer.h"
#include "../../Library/Component/Animator.h"
#include "../EnemyController.h"
#include "WeakController.h"
#include "../../Source/AI/MetaAI.h"

#include "StateMachine/WeakStateMachine.h"
#include "BehaviorTree/WeakBehaviorTree.h"

void WeakActor::OnCreate()
{
	EnemyActor::OnCreate();

	// モデル読み込み
	auto model = LoadModel(GetModelFilePath());

	// メタAI取得
	auto metaAIActor = GetScene()->GetActorManager().FindByName("MetaAI", ActorTag::DrawContextParameter);
	std::shared_ptr<MetaAI> metaAI = metaAIActor->GetComponent<MetaAI>();

	GetTransform().SetLengthScale(0.01f);
	GetTransform().SetScale(2.0f);
	_charactorController.lock()->SetMass(2.0f);

	// コンポーネント追加
	auto modelRenderer = AddComponent<ModelRenderer>();
	auto animator = AddComponent<Animator>();
	auto enemyController = AddComponent<EnemyController>();
	auto weakController = AddComponent<WeakController>();
	auto stateMachine = std::make_shared<WeakStateMachine>(
		enemyController.get(),
		weakController.get(),
		animator.get(),
		_damageable.lock().get());
	auto stateController = AddComponent<StateController>(stateMachine);
	auto behaviorController = AddComponent<BehaviorController>(std::make_shared<WeakBehaviorTree>(stateMachine.get(), animator.get(), metaAI.get()));
	auto effectController = this->AddComponent<EffectController>();
	effectController->LoadEffekseerEffect(0, "./Data/Effect/Effekseer/Player/Attack_Impact.efk");

	// コライダー追加
	auto modelCollider = AddCollider<ModelCollider>();
	modelCollider->SetLayer(CollisionLayer::Hit);

	// パラメータ設定
	enemyController->SetSearchRange(10.0f);
	enemyController->SetCombatRange(30.0f);
	enemyController->SetAttackRange(3.0f);
	enemyController->SetNearAttackRange(2.0f);
	enemyController->SetRotationSpeed(DirectX::XMConvertToRadians(1080.0f));
	enemyController->SetLookAtRadian(DirectX::XMConvertToRadians(20.0f));
}
