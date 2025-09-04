#pragma once

#include "../../Library/Algorithm/BehaviorTree/BehaviorActionBase.h"

// 前方宣言
class WyvernBehaviorTree;

/// <summary>
/// 指定のステートが完了するまで実行し続ける
/// </summary>
class WyvernCompleteStateAction : public BehaviorActionBase<WyvernBehaviorTree>
{
public:
	WyvernCompleteStateAction(
		WyvernBehaviorTree* owner,
		const char* startStateName,
		const char* endStateName = "Idle") :
		BehaviorActionBase(owner),
		_startStateName(startStateName),
		_endStateName(endStateName) {}
	// 開始処理
	void Enter()override;
	// 実行処理
	BehaviorActionState Execute(float elapsedTime) override;
private:
	const char* _startStateName = nullptr; // 実行するステート名
	const char* _endStateName = nullptr; // 実行するステート名
};

/// <summary>
/// 指定のステートを再生してすぐに終了する
/// </summary>
class WyvernOneAction : public BehaviorActionBase<WyvernBehaviorTree>
{
public:
	WyvernOneAction(WyvernBehaviorTree* owner, const char* stateName) :
		BehaviorActionBase(owner),
		_stateName(stateName) {
	}
	// 開始処理
	void Enter() override;
	// 実行処理
	BehaviorActionState Execute(float elapsedTime) override;
private:
	const char* _stateName = nullptr; // 実行するステート名
};
