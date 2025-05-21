#pragma once

#include "../../Library/Algorithm/BehaviorTree/BehaviorActionBase.h"

// ‘O•ûéŒ¾
class WyvernBehaviorTree;

// ’ÊíUŒ‚
class WyvernNormalAction : public BehaviorActionBase<WyvernBehaviorTree>
{
public:
	WyvernNormalAction(WyvernBehaviorTree* owner) :BehaviorActionBase(owner) {}
	BehaviorActionState Run(float elapsedTime) override;
};

//// œpœjs“®
//class WanderAction : public ActionBase
//{
//public:
//	WanderAction(EnemyBlueSlime* enemy) :ActionBase(enemy) {}
//	ActionBase::State Run(float elapsedTime);
//};

// ‘Ò‹@s“®
class WyvernIdleAction : public BehaviorActionBase<WyvernBehaviorTree>
{
public:
	WyvernIdleAction(WyvernBehaviorTree* owner) :BehaviorActionBase(owner) {}
	BehaviorActionState Run(float elapsedTime) override;
};
