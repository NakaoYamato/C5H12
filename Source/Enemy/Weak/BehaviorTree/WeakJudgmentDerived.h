#pragma once

#include "../../Library/Algorithm/BehaviorTree/BehaviorJudgmentBase.h"

// ‘O•ûéŒ¾
class WeakBehaviorTree;

// FightingNode‚É‘JˆÚ‚Å‚«‚é‚©”»’è
class WeakFightingJudgment : public BehaviorJudgmentBase<WeakBehaviorTree>
{
public:
	WeakFightingJudgment(WeakBehaviorTree* owner) : BehaviorJudgmentBase(owner) {};
	// ”»’è
	bool Judgment() override;
};
// BattleNode‚É‘JˆÚ‚Å‚«‚é‚©”»’è
class WeakBattleJudgment : public BehaviorJudgmentBase<WeakBehaviorTree>
{
public:
	WeakBattleJudgment(WeakBehaviorTree* owner) : BehaviorJudgmentBase(owner) {};
	// ”»’è
	bool Judgment() override;
};
// NearAttackNode‚É‘JˆÚ‚Å‚«‚é‚©”»’è
class WeakNearAttackJudgment : public BehaviorJudgmentBase<WeakBehaviorTree>
{
public:
	WeakNearAttackJudgment(WeakBehaviorTree* owner) : BehaviorJudgmentBase(owner) {};
	// ”»’è
	bool Judgment() override;
};
// JumpAttackNode‚É‘JˆÚ‚Å‚«‚é‚©”»’è
class WeakNearJumpAttackJudgment : public BehaviorJudgmentBase<WeakBehaviorTree>
{
public:
	WeakNearJumpAttackJudgment(WeakBehaviorTree* owner) : BehaviorJudgmentBase(owner) {};
	// ”»’è
	bool Judgment() override;
};
