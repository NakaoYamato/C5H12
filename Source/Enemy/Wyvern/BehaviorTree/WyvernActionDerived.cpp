#include "WyvernActionDerived.h"

#include "WyvernBehaviorTree.h"
#include "../WyvernEnemyController.h"

void WyvernStateAction::Enter()
{
	_owner->GetWyvern()->GetState()->GetStateMachine().ChangeState(_startStateName);
}

BehaviorActionState WyvernStateAction::Execute(float elapsedTime)
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
