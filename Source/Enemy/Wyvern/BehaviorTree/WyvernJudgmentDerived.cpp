#include "WyvernJudgmentDerived.h"

// alertNodeに遷移できるか判定
bool WyvernAlertJudgment::Judgment()
{
	return _owner->GetCombatStatus()->GetCurrentStatus() == CombatStatusController::Status::Alert;
}

// 回転が必要か判定
bool WyvernTurnJudgment::Judgment()
{
	// ターゲット方向に向いているか判定
	float angleToTarget = _owner->GetStateMachine()->GetEnemy()->GetAngleToTarget(
		_owner->GetCombatStatus()->GetTargetPosition());
	return (angleToTarget >= DirectX::XMConvertToRadians(90.0f));
}

// 咆哮に遷移できるか判定
bool WyvernRoarJudgment::Judgment()
{
	// 警戒状態が一定値以上になっていたら遷移
	return _owner->GetCombatStatus()->GetStatusTimer() >= _roarTime;
}

// angryNodeに遷移できるか判定
bool WyvernAngryJudgment::Judgment()
{
	bool res = false;

	if (_owner->GetStateMachine()->GetEnemy()->IsAngry() && !_wasAngry)
	{
		res = true;
	}

	_wasAngry = _owner->GetStateMachine()->GetEnemy()->IsAngry();

	return res;
}

// flightNodeに遷移できるか判定
bool WyvernFlightJudgment::Judgment()
{
	return _owner->GetStateMachine()->GetWyvern()->IsDuringFlight();
}

// BattleNode遷移判定
bool WyvernBattleJudgment::Judgment()
{
	// 戦闘状態でなければ遷移しない
	if (_owner->GetCombatStatus()->GetCurrentStatus() != CombatStatusController::Status::Combat)
		return false;

	float length = _owner->GetCombatStatus()->GetToTargetVec().Length();
	float searchRange = _owner->GetCombatStatus()->GetSearchRange();

	// 現在の位置とターゲットの位置の距離から索敵できるか判定
	return length < searchRange;
}

// ConfrontNodeに遷移できるか判定
bool WyvernConfrontJudgment::Judgment()
{
	auto& targetPosition = _owner->GetCombatStatus()->GetTargetPosition();
	// ターゲット方向に向いているか判定
	float angle = _owner->GetStateMachine()->GetEnemy()->GetAngleToTarget(targetPosition);
	// 向いてるならfalse
	if (angle < _owner->GetStateMachine()->GetEnemy()->GetLookAtRadian())
		return false;

	return true;
}

// AttackNodeに遷移できるか判定
bool WyvernAttackJudgment::Judgment()
{
	// スタミナが一定以下なら遷移しない
	if (_owner->GetStaminaController()->GetStamina() < WyvernBehaviorTree::RequiredStaminaForAttack)
		return false;

	// 一定確率で遷移しない
	// TODO : パラメータ化
	if (std::rand() % 10 == 0)
		return false;

	float length = _owner->GetCombatStatus()->GetToTargetVec().Length();
	float attackRange = _owner->GetStateMachine()->GetEnemy()->GetAttackRange();

	// 現在の位置とターゲットの位置の距離から攻撃できるか判定
	return length < attackRange;
}

// NearAttackNodeに遷移できるか判定
bool WyvernNearAttackJudgment::Judgment()
{
	auto vec = _owner->GetCombatStatus()->GetToTargetVec();
	float length = vec.Length();
	float nearAttackRange = _owner->GetStateMachine()->GetEnemy()->GetNearAttackRange();

	// 現在の位置とターゲットの位置の距離から近接攻撃できるか判定
	if (length < nearAttackRange)
	{
		// 自身の向いている方向にターゲットがいるか判定
		auto targetDirection = vec.Normalize();
		auto front = _owner->GetStateMachine()->GetWyvern()->GetActor()->GetTransform().GetAxisZ().Normalize();
		if (std::acosf(targetDirection.Dot(front)) < _owner->GetStateMachine()->GetWyvern()->GetNearAttackRadian())
			return true;
	}
	return false;
}

// WanderNode遷移判定
bool WyvernWanderJudgment::Judgment()
{
	return false;
}

// 滞空近距離判定
bool WyvernHoverNearJudgment::Judgment()
{
	float length = _owner->GetCombatStatus()->GetToTargetVec().Length();
	float range = _owner->GetStateMachine()->GetWyvern()->GetFlightNearRange();

	return length < range;
}

// 滞空終了判定
bool WyvernHoverEndJudgment::Judgment()
{
	// 滞空時間が経過しているかスタミナが無いなら遷移
	return _owner->GetStaminaController()->GetStamina() <= 0.0f ||
		_owner->GetStateMachine()->GetWyvern()->GetFlightTimer() >= _owner->GetStateMachine()->GetWyvern()->GetFlightDuration();
}

