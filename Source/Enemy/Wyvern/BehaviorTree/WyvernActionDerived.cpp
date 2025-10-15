#include "WyvernActionDerived.h"

#include "WyvernBehaviorTree.h"

void WyvernCompleteStateAction::Enter()
{
	_owner->GetStateMachine()->GetBase().ChangeState(_startStateName);
}

BehaviorActionState WyvernCompleteStateAction::Execute(float elapsedTime)
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

void WyvernOneAction::Enter()
{
	std::string currentStateName = _owner->GetStateMachine()->GetStateName();
	// 現在のステートが実行するステートと同じなら何もしない
	if (currentStateName == _stateName)
	{
		return;
	}
	// ステートを変更
	_owner->GetStateMachine()->GetBase().ChangeState(_stateName);
}

BehaviorActionState WyvernOneAction::Execute(float elapsedTime)
{
	// すぐに終了
	return BehaviorActionState::Complete;
}

// 終了処理
void WyvernRoarAction::Exit()
{
	// 咆哮が終わったら戦闘状態にする
	_owner->GetCombatStatus()->SetStatus(CombatStatusController::Status::Combat);
}
