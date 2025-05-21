#include "WyvernJudgmentDerived.h"

#include "../../Source/BehaviorTree/Enemy/Wyvern/WyvernBehaviorTree.h"
#include "../../Source/Component/Enemy/Wyvern/WyvernEnemyController.h"

// BattleNode遷移判定
bool WyvernBattleJudgment::Judgment()
{
	auto& position = _owner->GetWyvern()->GetActor()->GetTransform().GetPosition();
	auto& targetPosition = _owner->GetWyvern()->GetTargetPosition();
	float searchRange = _owner->GetWyvern()->GetSearchRange();

	// 現在の位置とターゲットの位置の距離から索敵できるか判定
	if ((position - targetPosition).Length() < searchRange)
	{
		return true;
	}

	return false;
}

// AttackNodeに遷移できるか判定
bool WyvernAttackJudgment::Judgment()
{
	auto& position = _owner->GetWyvern()->GetActor()->GetTransform().GetPosition();
	auto& targetPosition = _owner->GetWyvern()->GetTargetPosition();
	float attackRange = _owner->GetWyvern()->GetAttackRange();

	// 現在の位置とターゲットの位置の距離から攻撃できるか判定
	if ((position - targetPosition).Length() < attackRange)
	{
		return true;
	}

	return false;
}

bool WyvernWanderJudgment::Judgment()
{
	return false;
}
