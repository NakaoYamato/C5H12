#pragma once

#include "../../Library/Algorithm/BehaviorTree/BehaviorActionBase.h"

// 前方宣言
class WyvernBehaviorTree;

/// <summary>
/// 指定のステートが完了するまで実行し続ける
/// </summary>
class WyvernStateAction : public BehaviorActionBase<WyvernBehaviorTree>
{
public:
	WyvernStateAction(
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
