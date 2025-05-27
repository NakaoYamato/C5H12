#include "WyvernActionDerived.h"

#include "WyvernBehaviorTree.h"
#include "../WyvernEnemyController.h"

void WyvernCompleteStateAction::Enter()
{
	_owner->GetWyvern()->GetState()->GetStateMachine().ChangeState(_startStateName);
}

BehaviorActionState WyvernCompleteStateAction::Execute(float elapsedTime)
{
	std::string currentStateName = _owner->GetWyvern()->GetState()->GetStateName();

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
	std::string currentStateName = _owner->GetWyvern()->GetState()->GetStateName();
	// 現在のステートが実行するステートと同じなら何もしない
	if (currentStateName == _stateName)
	{
		return;
	}
	// ステートを変更
	_owner->GetWyvern()->GetState()->GetStateMachine().ChangeState(_stateName);
}

BehaviorActionState WyvernOneAction::Execute(float elapsedTime)
{
	// すぐに終了
	return BehaviorActionState::Complete;
}
