#include "WeakJudgmentDerived.h"

#include "WeakBehaviorTree.h"
#include "../WeakController.h"

// FightingNode‘JˆÚ”»’è
bool WeakFightingJudgment::Judgment()
{
	// í“¬ó‘Ô‚È‚ç‘JˆÚ
	return _owner->GetStateMachine()->GetEnemy()->InFighting();
}
// BattleNode‘JˆÚ”»’è
bool WeakBattleJudgment::Judgment()
{
	// UŒ‚ƒN[ƒ‹ƒ^ƒCƒ€’†‚Í‘JˆÚ‚µ‚È‚¢
	if (_owner->GetStateMachine()->GetEnemy()->IsAttackCooldown())
		return false;

	auto& targetPosition = _owner->GetStateMachine()->GetEnemy()->GetTargetPosition();
	float combatRange = _owner->GetStateMachine()->GetEnemy()->GetCombatRange();

	// í“¬Œp‘±”ÍˆÍ“à‚È‚ç‘JˆÚ
	return _owner->GetStateMachine()->GetEnemy()->IsInTargetRange(targetPosition, combatRange);
}
// NearAttackNode ‘JˆÚ”»’è
bool WeakNearAttackJudgment::Judgment()
{
	auto& targetPosition = _owner->GetStateMachine()->GetEnemy()->GetTargetPosition();
	float nearAttackRange = _owner->GetStateMachine()->GetEnemy()->GetNearAttackRange();
	float lookAtRadian = _owner->GetStateMachine()->GetEnemy()->GetLookAtRadian();

	bool isTargetInRange = _owner->GetStateMachine()->GetEnemy()->IsInTargetRange(targetPosition, nearAttackRange);
	bool isLookAtTarget = _owner->GetStateMachine()->GetEnemy()->GetAngleToTarget(targetPosition) < lookAtRadian;

	// Œ»Ý‚ÌˆÊ’u‚Æƒ^[ƒQƒbƒg‚ÌˆÊ’u‚Ì‹——£‚©‚ç‹ßÚUŒ‚‚Å‚«‚é‚©”»’è
	return isTargetInRange && isLookAtTarget;
}
// JumpAttackNode ‘JˆÚ”»’è
bool WeakNearJumpAttackJudgment::Judgment()
{
	auto& targetPosition = _owner->GetStateMachine()->GetEnemy()->GetTargetPosition();
	float attackRange = _owner->GetStateMachine()->GetEnemy()->GetAttackRange();
	float lookAtRadian = _owner->GetStateMachine()->GetEnemy()->GetLookAtRadian();

	bool isTargetInRange = _owner->GetStateMachine()->GetEnemy()->IsInTargetRange(targetPosition, attackRange);
	bool isLookAtTarget = _owner->GetStateMachine()->GetEnemy()->GetAngleToTarget(targetPosition) < lookAtRadian;

	// Œ»Ý‚ÌˆÊ’u‚Æƒ^[ƒQƒbƒg‚ÌˆÊ’u‚Ì‹——£‚©‚ç‹ßÚUŒ‚‚Å‚«‚é‚©”»’è
	return isTargetInRange && isLookAtTarget;
}
