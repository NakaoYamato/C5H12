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

//// 徘徊行動
//class WanderAction : public ActionBase
//{
//public:
//	WanderAction(EnemyBlueSlime* enemy) :ActionBase(enemy) {}
//	ActionBase::State Run(float elapsedTime);
//};

// 待機行動
class WyvernIdleAction : public BehaviorActionBase<WyvernBehaviorTree>
{
public:
	WyvernIdleAction(WyvernBehaviorTree* owner) :BehaviorActionBase(owner) {}
	BehaviorActionState Run(float elapsedTime) override;
};
