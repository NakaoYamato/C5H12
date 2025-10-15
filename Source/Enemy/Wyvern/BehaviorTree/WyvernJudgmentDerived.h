#pragma once

#include "../../Library/Algorithm/BehaviorTree/BehaviorJudgmentBase.h"

// ‘O•ûéŒ¾
class WyvernBehaviorTree;

// alertNode‚É‘JˆÚ‚Å‚«‚é‚©”»’è
class WyvernAlertJudgment : public BehaviorJudgmentBase<WyvernBehaviorTree>
{
public:
	WyvernAlertJudgment(WyvernBehaviorTree* owner) :BehaviorJudgmentBase(owner) {};
	// ”»’è
	bool Judgment() override;
};

// ™ôšK‚É‘JˆÚ‚Å‚«‚é‚©”»’è
class WyvernRoarJudgment : public BehaviorJudgmentBase<WyvernBehaviorTree>
{
public:
	WyvernRoarJudgment(WyvernBehaviorTree* owner) :BehaviorJudgmentBase(owner) {};
	// ”»’è
	bool Judgment() override;
private:
	bool _wasInFighting = false;
};

// BattleNode‚É‘JˆÚ‚Å‚«‚é‚©”»’è
class WyvernBattleJudgment : public BehaviorJudgmentBase<WyvernBehaviorTree>
{
public:
	WyvernBattleJudgment(WyvernBehaviorTree* owner) :BehaviorJudgmentBase(owner) {};
	// ”»’è
	bool Judgment() override;
};

// ConfrontNode‚É‘JˆÚ‚Å‚«‚é‚©”»’è
class WyvernConfrontJudgment : public BehaviorJudgmentBase<WyvernBehaviorTree>
{
public:
	WyvernConfrontJudgment(WyvernBehaviorTree* owner) :BehaviorJudgmentBase(owner) {};
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

// NearAttackNode‚É‘JˆÚ‚Å‚«‚é‚©”»’è
class WyvernNearAttackJudgment :public BehaviorJudgmentBase<WyvernBehaviorTree>
{
public:
	WyvernNearAttackJudgment(WyvernBehaviorTree* owner) :BehaviorJudgmentBase(owner) {};
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

