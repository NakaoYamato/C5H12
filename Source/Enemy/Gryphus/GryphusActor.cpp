#include "GryphusActor.h"

#include "../../Library/Scene/Scene.h"
#include "../../Library/Component/Collider/ModelCollider.h"
#include "../../Library/Component/ModelRenderer.h"
#include "../../Library/Component/Animator.h"
#include "../EnemyController.h"
#include "GryphusController.h"

#include "../../Source/AI/MetaAI.h"
#include "StateMachine/GryphusStateMachine.h"

void GryphusActor::OnCreate()
{
    EnemyActor::OnCreate();

    // モデル読み込み
    auto model = LoadModel(GetModelFilePath());

    // メタAI取得
    auto metaAIActor = GetScene()->GetActorManager().FindByName("MetaAI", ActorTag::DrawContextParameter);
    std::shared_ptr<MetaAI> metaAI = metaAIActor->GetComponent<MetaAI>();

    GetTransform().SetLengthScale(0.1f);
    GetTransform().SetScale(1.3f);
    _charactorController.lock()->SetMass(100.0f);

    // コンポーネント追加
    auto modelRenderer = AddComponent<ModelRenderer>();
    auto animator = AddComponent<Animator>();
    auto enemyController = AddComponent<EnemyController>();
    auto gryphusController = AddComponent<GryphusController>();
    auto stateMachine = std::make_shared<GryphusStateMachine>(
        enemyController.get(),
        gryphusController.get(),
        animator.get(),
        _damageable.lock().get());
    auto stateController = AddComponent<StateController>(stateMachine);
    //auto behaviorController = AddComponent<BehaviorController>(std::make_shared<WeakBehaviorTree>(stateMachine.get(), _animator.get(), metaAI.get()));

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
