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

#pragma region 指定のステートを再生してすぐに終了する
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
