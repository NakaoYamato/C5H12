#include "WyvernActionDerived.h"

#include "WyvernBehaviorTree.h"

#pragma region 指定のステートが完了するまで実行し続ける
void WyvernCompleteStateAction::Enter()
{
	_owner->GetStateMachine()->GetBase().ChangeState(_startStateName);
}

BehaviorActionState WyvernCompleteStateAction::Execute(float elapsedTime)
{
	std::string currentStateName = _owner->GetStateMachine()->GetStateName();

	if (currentStateName == _endStateName)
	{
		// ステートが終了ステートと同じなら成功
		return BehaviorActionState::Complete;
	}
	// それ以外は実行中
	return BehaviorActionState::Run;
}
#pragma endregion

#pragma region 指定のサブステートが完了するまで実行し続ける
// 開始処理
void WyvernCompleteSubStateAction::Enter()
{
	_owner->GetStateMachine()->ChangeState(_startStateName, _startSubStateName);
}
// 実行処理
BehaviorActionState WyvernCompleteSubStateAction::Execute(float elapsedTime)
{
	std::string currentStateName = _owner->GetStateMachine()->GetStateName();

	if (currentStateName == _endStateName)
	{
		// ステートが終了ステートと同じなら成功
		return BehaviorActionState::Complete;
	}
	// それ以外は実行中
	return BehaviorActionState::Run;
}
#pragma endregion

#pragma region 咆哮アクション
// 終了処理
void WyvernRoarAction::Exit()
{
	// 咆哮が終わったら戦闘状態にする
	_owner->GetCombatStatus()->SetStatus(CombatStatusController::Status::Combat);
}
#pragma endregion

#pragma region 指定時間再生するアクション
// 開始処理
void WyvernTimerAction::Enter()
{
	_owner->GetStateMachine()->GetBase().ChangeState(_startStateName);
	_timer = 0.0f;
}

// 実行処理
BehaviorActionState WyvernTimerAction::Execute(float elapsedTime)
{
	_timer += elapsedTime;
	if (_timer >= _time)
	{
		return BehaviorActionState::Complete;
	}
	return BehaviorActionState::Run;
}
#pragma endregion
