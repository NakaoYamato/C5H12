#include "WyvernJudgmentDerived.h"

#include "WyvernBehaviorTree.h"
#include "../WyvernController.h"
#include "../../EnemyController.h"

// BattleNode遷移判定
bool WyvernBattleJudgment::Judgment()
{
	// 戦闘状態でなければ遷移しない
	if (!_owner->GetStateMachine()->GetEnemy()->InFighting())
		return false;

	auto& position = _owner->GetStateMachine()->GetWyvern()->GetActor()->GetTransform().GetPosition();
	auto& targetPosition = _owner->GetStateMachine()->GetEnemy()->GetTargetPosition();
	float searchRange = _owner->GetStateMachine()->GetEnemy()->GetSearchRange();

	// 現在の位置とターゲットの位置の距離から索敵できるか判定
	return (position - targetPosition).Length() < searchRange;
}

// ConfrontNodeに遷移できるか判定
bool WyvernConfrontJudgment::Judgment()
{
	auto& targetPosition = _owner->GetStateMachine()->GetEnemy()->GetTargetPosition();
	// ターゲット方向に向いているか判定
	float angle = _owner->GetStateMachine()->GetEnemy()->GetAngleToTarget(targetPosition);
	// 向いてるならfalse
	if (angle < _owner->GetStateMachine()->GetWyvern()->GetLookAtRadian())
		return false;

	return true;
}

// AttackNodeに遷移できるか判定
bool WyvernAttackJudgment::Judgment()
{
	// 一定確率で遷移しない
	// TODO : パラメータ化
	if (std::rand() % 10 == 0)
		return false;

	auto& position = _owner->GetStateMachine()->GetWyvern()->GetActor()->GetTransform().GetPosition();
	auto& targetPosition = _owner->GetStateMachine()->GetEnemy()->GetTargetPosition();
	float attackRange = _owner->GetStateMachine()->GetEnemy()->GetAttackRange();

	// 現在の位置とターゲットの位置の距離から攻撃できるか判定
	return (position - targetPosition).Length() < attackRange;
}

// NearAttackNodeに遷移できるか判定
bool WyvernNearAttackJudgment::Judgment()
{
	auto& position = _owner->GetStateMachine()->GetWyvern()->GetActor()->GetTransform().GetPosition();
	auto& targetPosition = _owner->GetStateMachine()->GetEnemy()->GetTargetPosition();
	float nearAttackRange = _owner->GetStateMachine()->GetWyvern()->GetNearAttackRange();

	// 現在の位置とターゲットの位置の距離から近接攻撃できるか判定
	if ((position - targetPosition).Length() < nearAttackRange)
	{
		// 自身の向いている方向にターゲットがいるか判定
		auto targetDirection = (targetPosition - position).Normalize();
		auto front = _owner->GetStateMachine()->GetWyvern()->GetActor()->GetTransform().GetAxisZ().Normalize();
		if (std::acosf(targetDirection.Dot(front)) < _owner->GetStateMachine()->GetWyvern()->GetNearAttackRadian())
			return true;
	}
	return false;
}

bool WyvernWanderJudgment::Judgment()
{
	return false;
}
