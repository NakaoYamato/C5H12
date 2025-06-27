#include "WyvernBehaviorController.h"

#include "../../Library/Scene/Scene.h"
#include "../../Library/Component/Animator.h"
#include "StateMachine/WyvernStateMachine.h"

#include <imgui.h>

// 開始処理
void WyvernBehaviorController::Start()
{
	_enemyController = GetActor()->GetComponent<EnemyController>();
	auto animator = GetActor()->GetComponent<Animator>();
	auto stateController = GetActor()->GetComponent<StateController>();
	assert(_enemyController.lock());
	assert(animator);
	assert(stateController);

	auto wyvernStateMachine = std::dynamic_pointer_cast<WyvernStateMachine>(stateController->GetStateMachine());

	// ビヘイビアツリー作成
	_behaviorTree = std::make_unique<WyvernBehaviorTree>(wyvernStateMachine.get(), animator.get());

	// メタAIを取得
	auto metaAIActor = GetActor()->GetScene()->GetActorManager().FindByName("MetaAI", ActorTag::DrawContextParameter);
	if (metaAIActor)
	{
		_metaAI = metaAIActor->GetComponent<MetaAI>();
	}
}

// 更新処理
void WyvernBehaviorController::Update(float elapsedTime)
{
	if (_isExecuteBehavior)
	{
		// メタAIからターゲット座標を取得
		if (auto metaAI = _metaAI.lock())
		{
			auto targetable = metaAI->SearchTarget(
				Targetable::Faction::Player,
				GetActor()->GetTransform().GetWorldPosition(),
				100.0f);
			if (targetable)
			{
				_enemyController.lock()->SetTargetPosition(targetable->GetActor()->GetTransform().GetWorldPosition());
			}
		}

		// ビヘイビアツリーの実行
		_behaviorTree->Execute(elapsedTime);
	}
}

// GUI描画
void WyvernBehaviorController::DrawGui()
{
	bool flag = _isExecuteBehavior;
	ImGui::Checkbox(u8"ビヘイビアツリーを実行する", &flag);
	_behaviorTree->DrawGui();
}
