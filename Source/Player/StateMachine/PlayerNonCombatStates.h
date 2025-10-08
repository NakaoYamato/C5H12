#pragma once

#include "PlayerStateMachine.h"

// 納刀状態のプレイヤーステート

#pragma region 待機
class PlayerNonCombatIdleState final : public PlayerHSB
{
public:
	PlayerNonCombatIdleState(PlayerStateMachine* stateMachine) :
		PlayerHSB(stateMachine, u8"Idle", 0.2f, true, false) {
	}
	~PlayerNonCombatIdleState() override {}
	// ステート名取得
	const char* GetName() const override { return "Idle"; }
	void OnExecute(float elapsedTime) override;
};
#pragma endregion

#pragma region 歩き
class PlayerNonCombatWalkState : public HierarchicalStateBase<PlayerStateMachine>
{
public:
	PlayerNonCombatWalkState(PlayerStateMachine* stateMachine);
	~PlayerNonCombatWalkState() override {}
	// ステート名取得
	const char* GetName() const override { return "Walk"; }
	void OnEnter() override;
	void OnExecute(float elapsedTime) override;
	void OnExit() override;
};
#pragma endregion

#pragma region 走り
class PlayerNonCombatRunState : public HierarchicalStateBase<PlayerStateMachine>
{
public:
	PlayerNonCombatRunState(PlayerStateMachine* stateMachine);
	~PlayerNonCombatRunState() override {}
	// ステート名取得
	const char* GetName() const override { return "Run"; }
	void OnEnter() override;
	void OnExecute(float elapsedTime) override;
	void OnExit() override;
};
#pragma endregion

#pragma region 回避
class PlayerNonCombatEvadeState final : public Player8WayHSB
{
public:
	PlayerNonCombatEvadeState(PlayerStateMachine* stateMachine);
	~PlayerNonCombatEvadeState() override {}

	// ステート名取得
	const char* GetName() const override { return "Evade"; }
	void OnEnter() override;
	void OnExecute(float elapsedTime) override;
	void OnExit() override {}
};
#pragma endregion

#pragma region 抜刀
class PlayerNonCombatToCombatState  final : public PlayerHSB
{
public:
	PlayerNonCombatToCombatState(PlayerStateMachine* stateMachine) :
		PlayerHSB(stateMachine, u8"IdleToIdleCombat", 0.2f, false, true) {
	}
	~PlayerNonCombatToCombatState() override {}
	// ステート名取得
	const char* GetName() const override { return "ToCombat"; }
	void OnExecute(float elapsedTime) override;
};
#pragma endregion

#pragma region 死亡
class PlayerNonCombatDeathState   final : public PlayerHSB
{
public:
	PlayerNonCombatDeathState(PlayerStateMachine* stateMachine) :
		PlayerHSB(stateMachine, u8"HitCombatDeath", 0.2f, false, true) 
	{}
	~PlayerNonCombatDeathState() override {}
	// ステート名取得
	const char* GetName() const override { return "Death"; }
	void OnEnter() override {}
	void OnExecute(float elapsedTime) override {}
	void OnExit() override {}
};
#pragma endregion