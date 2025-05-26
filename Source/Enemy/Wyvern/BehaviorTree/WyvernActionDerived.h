#pragma once

#include "../../Library/Algorithm/BehaviorTree/BehaviorActionBase.h"

// 前方宣言
class WyvernBehaviorTree;

// 通常攻撃
class WyvernNormalAction : public BehaviorActionBase<WyvernBehaviorTree>
{
public:
	WyvernNormalAction(WyvernBehaviorTree* owner) :BehaviorActionBase(owner) {}
	// 開始処理
	void OnEnter()override;
	// 実行処理
	BehaviorActionState Run(float elapsedTime) override;
};

#pragma region 噛みつき攻撃
class WyvernBiteAction : public BehaviorActionBase<WyvernBehaviorTree>
{
public:
	WyvernBiteAction(WyvernBehaviorTree* owner) :BehaviorActionBase(owner) {}
	// 開始処理
	void OnEnter()override;
	// 実行処理
	BehaviorActionState Run(float elapsedTime) override;
};
#pragma endregion

#pragma region かぎ爪攻撃
class WyvernClawAction : public BehaviorActionBase<WyvernBehaviorTree>
{
public:
	WyvernClawAction(WyvernBehaviorTree* owner) :BehaviorActionBase(owner) {}
	// 開始処理
	void OnEnter()override;
	// 実行処理
	BehaviorActionState Run(float elapsedTime) override;
};
#pragma endregion

#pragma region 尻尾攻撃
class WyvernTailAction : public BehaviorActionBase<WyvernBehaviorTree>
{
public:
	WyvernTailAction(WyvernBehaviorTree* owner) :BehaviorActionBase(owner) {}
	// 開始処理
	void OnEnter()override;
	// 実行処理
	BehaviorActionState Run(float elapsedTime) override;
};
#pragma endregion

#pragma region 後退
class WyvernBackStepAction : public BehaviorActionBase<WyvernBehaviorTree>
{
public:
	WyvernBackStepAction(WyvernBehaviorTree* owner) :BehaviorActionBase(owner) {}
	// 開始処理
	void OnEnter()override;
	// 実行処理
	BehaviorActionState Run(float elapsedTime) override;
};
#pragma endregion

#pragma region ブレス
class WyvernBreathAction : public BehaviorActionBase<WyvernBehaviorTree>
{
public:
	WyvernBreathAction(WyvernBehaviorTree* owner) :BehaviorActionBase(owner) {}
	// 開始処理
	void OnEnter()override;
	// 実行処理
	BehaviorActionState Run(float elapsedTime) override;
};
#pragma endregion

#pragma region とびかかり
class WyvernJumpOnAction : public BehaviorActionBase<WyvernBehaviorTree>
{
public:
	WyvernJumpOnAction(WyvernBehaviorTree* owner) :BehaviorActionBase(owner) {}
	// 開始処理
	void OnEnter()override;
	// 実行処理
	BehaviorActionState Run(float elapsedTime) override;
};
#pragma endregion

#pragma region 近づく
class WyvernPursuitAction : public BehaviorActionBase<WyvernBehaviorTree>
{
public:
	WyvernPursuitAction(WyvernBehaviorTree* owner) :BehaviorActionBase(owner) {}
	// 開始処理
	void OnEnter()override;
	// 実行処理
	BehaviorActionState Run(float elapsedTime) override;
};
#pragma endregion


//// 徘徊行動
//class WanderAction : public ActionBase
//{
//public:
//	WanderAction(EnemyBlueSlime* enemy) :ActionBase(enemy) {}
//	ActionBase::State Run(float elapsedTime);
//};

#pragma region 待機
class WyvernIdleAction : public BehaviorActionBase<WyvernBehaviorTree>
{
public:
	WyvernIdleAction(WyvernBehaviorTree* owner) :BehaviorActionBase(owner) {}
	// 開始処理
	void OnEnter()override;
	// 実行処理
	BehaviorActionState Run(float elapsedTime) override;
};
#pragma endregion
