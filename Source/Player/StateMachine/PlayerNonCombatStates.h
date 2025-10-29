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

#pragma region 振り向き
class PlayerNonCombatTurnState final : public HierarchicalStateBase<PlayerStateMachine>
{
public:
	PlayerNonCombatTurnState(PlayerStateMachine* stateMachine);
	~PlayerNonCombatTurnState() override {}
	// ステート名取得
	const char* GetName() const override { return "Turn"; }
	void OnEnter() override;
	void OnExecute(float elapsedTime) override;
	void OnExit() override;
};
#pragma endregion

#pragma region 歩き
class PlayerNonCombatWalkState final : public HierarchicalStateBase<PlayerStateMachine>
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
class PlayerNonCombatRunState final : public HierarchicalStateBase<PlayerStateMachine>
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
	void OnExit() override;
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

#pragma region 被弾
class PlayerNonCombatHitState final : public Player8WayHSB
{
public:
	PlayerNonCombatHitState(PlayerStateMachine* stateMachine);
	~PlayerNonCombatHitState() override {}
	// ステート名取得
	const char* GetName() const override { return "Hit"; }
	void OnEnter() override;
	void OnExecute(float elapsedTime) override;
	void OnExit() override;
};
class PlayerNonCombatHitKnockDownState final : public Player8WayHSB
{
public:
	PlayerNonCombatHitKnockDownState(PlayerStateMachine* stateMachine);
	~PlayerNonCombatHitKnockDownState() override {}
	// ステート名取得
	const char* GetName() const override { return "HitKnockDown"; }
	void OnEnter() override;
	void OnExecute(float elapsedTime) override;
	void OnExit() override;
};
#pragma endregion

#pragma region ダウン
class PlayerNonCombatDownState final : public HierarchicalStateBase<PlayerStateMachine>
{
public:
	PlayerNonCombatDownState(PlayerStateMachine* stateMachine);
	~PlayerNonCombatDownState() override {}
	// ステート名取得
	const char* GetName() const override { return "Down"; }
	void OnEnter() override;
	void OnExecute(float elapsedTime) override {}
	void OnExit() override;
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
