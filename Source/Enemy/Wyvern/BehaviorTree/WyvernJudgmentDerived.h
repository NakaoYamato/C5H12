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

// ‰ñ“]‚ª•K—v‚©”»’è
class WyvernTurnJudgment : public BehaviorJudgmentBase<WyvernBehaviorTree>
{
public:
	WyvernTurnJudgment(WyvernBehaviorTree* owner) :BehaviorJudgmentBase(owner) {};
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
	float _roarTime = 3.0f;
};

// angryNode‚É‘JˆÚ‚Å‚«‚é‚©”»’è
class WyvernAngryJudgment : public BehaviorJudgmentBase<WyvernBehaviorTree>
{
public:
	WyvernAngryJudgment(WyvernBehaviorTree* owner) :BehaviorJudgmentBase(owner) {};
	// ”»’è
	bool Judgment() override;
private:
	bool _wasAngry = false;
};

// flightNode‚É‘JˆÚ‚Å‚«‚é‚©”»’è
class WyvernFlightJudgment : public BehaviorJudgmentBase<WyvernBehaviorTree>
{
public:
	WyvernFlightJudgment(WyvernBehaviorTree* owner) :BehaviorJudgmentBase(owner) {};
	// ”»’è
	bool Judgment() override;
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

// ‘Ø‹ó‹ß‹——£”»’è
class WyvernHoverNearJudgment : public BehaviorJudgmentBase<WyvernBehaviorTree>
{
public:
	WyvernHoverNearJudgment(WyvernBehaviorTree* owner) :BehaviorJudgmentBase(owner) {};
	// ”»’è
	bool Judgment() override;
};

// ‘Ø‹óI—¹”»’è
class WyvernHoverEndJudgment : public BehaviorJudgmentBase<WyvernBehaviorTree>
{
public:
	WyvernHoverEndJudgment(WyvernBehaviorTree* owner) :BehaviorJudgmentBase(owner) {};
	// ”»’è
	bool Judgment() override;
};
