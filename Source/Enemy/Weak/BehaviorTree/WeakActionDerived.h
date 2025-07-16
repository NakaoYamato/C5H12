#pragma once

#include "../../Library/Algorithm/BehaviorTree/BehaviorActionBase.h"

// 前方宣言
class WeakBehaviorTree;

/// <summary>
/// 指定のステートが完了するまで実行し続ける
/// </summary>
class WeakCompleteStateAction : public BehaviorActionBase<WeakBehaviorTree>
{
public:
	WeakCompleteStateAction(
		WeakBehaviorTree* owner,
		const char* startStateName,
		const char* endStateName = "Idle") :
		BehaviorActionBase(owner),
		_startStateName(startStateName),
		_endStateName(endStateName) {
	}
	// 開始処理
	void Enter()override;
	// 実行処理
	BehaviorActionState Execute(float elapsedTime) override;
private:
	const char* _startStateName = nullptr; // 実行するステート名
	const char* _endStateName = nullptr; // 終了先のステート名
};
class WeakCompleteStatePursuitAction : public WeakCompleteStateAction
{
public:
	WeakCompleteStatePursuitAction(
		WeakBehaviorTree* owner,
		const char* startStateName,
		const char* endStateName = "Idle") :
		WeakCompleteStateAction(owner, startStateName, endStateName) {
	}
	// 実行処理
	BehaviorActionState Execute(float elapsedTime) override;
};
/// <summary>
/// 指定のステートが完了するまで実行し続け、途中でターゲットがいたら成功を返す
/// </summary>
class WeakSearchTargetAction : public BehaviorActionBase<WeakBehaviorTree>
{
public:
	WeakSearchTargetAction(WeakBehaviorTree* owner,
		const char* executeStateName,
		const char* endStateName = "Idle") :
		BehaviorActionBase(owner),
		_executeStateName(executeStateName),
		_endStateName(endStateName)
	{}
	// 開始処理
	void Enter() override;
	// 実行処理
	BehaviorActionState Execute(float elapsedTime) override;
private:
	const char* _executeStateName = nullptr; // 実行するステート名
	const char* _endStateName = nullptr; // 終了先のステート名
};