#include "WeakActionDerived.h"
#include "WeakBehaviorTree.h"

#include "../../Source/Enemy/EnemyController.h"

void WeakCompleteStateAction::Enter()
{
	_owner->GetStateMachine()->GetBase().ChangeState(_startStateName);
}

BehaviorActionState WeakCompleteStateAction::Execute(float elapsedTime)
{
	std::string currentStateName = _owner->GetStateMachine()->GetStateName();

	if (currentStateName == _endStateName)
	{
		// ステートが開始ステートと同じなら成功
		return BehaviorActionState::Complete;
	}
	// それ以外は実行中
	return BehaviorActionState::Run;
}

BehaviorActionState WeakCompleteStatePursuitAction::Execute(float elapsedTime)
{
	auto enemy = _owner->GetStateMachine()->GetEnemy();
	auto position = enemy->GetActor()->GetTransform().GetWorldPosition();
	float searchRange = _owner->GetCombatStatus()->GetSearchRange();

	// メタAIからターゲット座標を取得
	auto targetable = _owner->GetMetaAI()->SearchTarget(
		Targetable::Faction::Player,
		position,
		searchRange);
	if (targetable)
	{
		_owner->GetCombatStatus()->SetTargetPosition(targetable->GetActor()->GetTransform().GetWorldPosition());
	}

	return WeakCompleteStateAction::Execute(elapsedTime);
}

void WeakSearchTargetAction::Enter()
{
	// 移動ステートに遷移
	_owner->GetStateMachine()->GetBase().ChangeState(_executeStateName);
}

BehaviorActionState WeakSearchTargetAction::Execute(float elapsedTime)
{
	auto enemy = _owner->GetStateMachine()->GetEnemy();
	auto position = enemy->GetActor()->GetTransform().GetWorldPosition();
	float searchRange = _owner->GetCombatStatus()->GetSearchRange();

	// メタAIからターゲット座標を取得
	auto targetable = _owner->GetMetaAI()->SearchTarget(
		Targetable::Faction::Player,
		position,
		searchRange);
	if (targetable)
	{
		// 攻撃対象が範囲内にいたら成功
		return BehaviorActionState::Complete;
	}
	std::string currentStateName = _owner->GetStateMachine()->GetStateName();

	if (currentStateName == _endStateName)
	{
		// ステートが開始ステートと同じなら成功
		return BehaviorActionState::Complete;
	}

	// それ以外は実行中
	return BehaviorActionState::Run;
}
