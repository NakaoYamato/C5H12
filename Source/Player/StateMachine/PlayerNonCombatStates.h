#pragma once

#include "PlayerStateMachine.h"

// 納刀状態のプレイヤーステート

#pragma region 待機
class PlayerNonCombatIdleState : public HierarchicalStateBase<PlayerStateMachine>
{
public:
	PlayerNonCombatIdleState(PlayerStateMachine* stateMachine) : HierarchicalStateBase(stateMachine) {}
	~PlayerNonCombatIdleState() override {}

	// ステート名取得
	const char* GetName() const override { return "NonCombatIdle"; }
	void OnEnter() override;
	void OnExecute(float elapsedTime) override;
	void OnExit() override {}
};
#pragma endregion

#pragma region 歩き
class PlayerNonCombatWalkState : public HierarchicalStateBase<PlayerStateMachine>
{
public:
	PlayerNonCombatWalkState(PlayerStateMachine* stateMachine) : HierarchicalStateBase(stateMachine) {}
	~PlayerNonCombatWalkState() override {}
	// ステート名取得
	const char* GetName() const override { return "NonCombatWalk"; }
	void OnEnter() override;
	void OnExecute(float elapsedTime) override;
	void OnExit() override {}
};
#pragma endregion

#pragma region 走り
class PlayerNonCombatRunState : public HierarchicalStateBase<PlayerStateMachine>
{
public:
	PlayerNonCombatRunState(PlayerStateMachine* stateMachine) : HierarchicalStateBase(stateMachine) {}
	~PlayerNonCombatRunState() override {}
	// ステート名取得
	const char* GetName() const override { return "NonCombatRun"; }
	void OnEnter() override;
	void OnExecute(float elapsedTime) override;
	void OnExit() override {}
};
#pragma endregion

