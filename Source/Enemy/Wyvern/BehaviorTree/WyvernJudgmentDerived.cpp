#include "WyvernJudgmentDerived.h"

#include "WyvernBehaviorTree.h"
#include "../WyvernEnemyController.h"

// BattleNode遷移判定
bool WyvernBattleJudgment::Judgment()
{
	auto& position = _owner->GetWyvern()->GetActor()->GetTransform().GetPosition();
	auto& targetPosition = _owner->GetWyvern()->GetTargetPosition();
	float searchRange = _owner->GetWyvern()->GetSearchRange();

	// 現在の位置とターゲットの位置の距離から索敵できるか判定
	return (position - targetPosition).Length() < searchRange;
}

// AttackNodeに遷移できるか判定
bool WyvernAttackJudgment::Judgment()
{
	auto& position = _owner->GetWyvern()->GetActor()->GetTransform().GetPosition();
	auto& targetPosition = _owner->GetWyvern()->GetTargetPosition();
	float attackRange = _owner->GetWyvern()->GetAttackRange();

	// 現在の位置とターゲットの位置の距離から攻撃できるか判定
	return (position - targetPosition).Length() < attackRange;
}

// NearAttackNodeに遷移できるか判定
bool WyvernNearAttackJudgment::Judgment()
{
	auto& position = _owner->GetWyvern()->GetActor()->GetTransform().GetPosition();
	auto& targetPosition = _owner->GetWyvern()->GetTargetPosition();
	float nearAttackRange = _owner->GetWyvern()->GetNearAttackRange();

	// 現在の位置とターゲットの位置の距離から近接攻撃できるか判定
	if ((position - targetPosition).Length() < nearAttackRange)
	{
		// 自身の向いている方向にターゲットがいるか判定
		auto targetDirection = (targetPosition - position).Normalize();
		auto front = _owner->GetWyvern()->GetActor()->GetTransform().GetAxisZ().Normalize();
		if (std::acosf(targetDirection.Dot(front)) < _owner->GetWyvern()->GetNearAttackRadian())
			return true;
	}
	return false;
}

bool WyvernWanderJudgment::Judgment()
{
	return false;
}
