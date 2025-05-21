#pragma once

#include "../../Library/Algorithm/BehaviorTree/BehaviorJudgmentBase.h"

// ‘O•ûéŒ¾
class WyvernBehaviorTree;

// BattleNode‘JˆÚ”»’è
class WyvernBattleJudgment : public BehaviorJudgmentBase<WyvernBehaviorTree>
{
public:
	WyvernBattleJudgment(WyvernBehaviorTree* owner) :BehaviorJudgmentBase(owner) {};
	// ”»’è
	bool Judgment() override;
};

// AttackNode‚É‘JˆÚ‚Å‚«‚é‚©”»’è
class WyvernAttackJudgment :public BehaviorJudgmentBase<WyvernBehaviorTree>
{
public:
	WyvernAttackJudgment(WyvernBehaviorTree* owner) :BehaviorJudgmentBase(owner) {};
	// ”»’è
	bool Judgment() override;
};

// WanderNode‘JˆÚ”»’è
class WyvernWanderJudgment : public BehaviorJudgmentBase<WyvernBehaviorTree>
{
public:
	WyvernWanderJudgment(WyvernBehaviorTree* owner) :BehaviorJudgmentBase(owner) {};
	// ”»’è
	bool Judgment() override;
};

