#pragma once

#include "PlayerStateMachine.h"

// 大剣装備状態のプレイヤーステート
#pragma region 待機
class PlayerGreatSwordIdleState final : public PlayerHSB
{
public:
	PlayerGreatSwordIdleState(PlayerStateMachine* stateMachine) : 
		PlayerHSB(stateMachine, u8"IdleCombat", 0.2f, true, false) {}
	~PlayerGreatSwordIdleState() override {}
	// ステート名取得
	const char* GetName() const override { return "CombatIdle"; }
	void OnExecute(float elapsedTime) override;
};
#pragma endregion

#pragma region 走り
class PlayerGreatSwordRunState final : public HierarchicalStateBase<PlayerStateMachine>
{
public:
	PlayerGreatSwordRunState(PlayerStateMachine* stateMachine);
	~PlayerGreatSwordRunState() override {}
	// ステート名取得
	const char* GetName() const override { return "CombatRun"; }
	void OnEnter() override;
	void OnExecute(float elapsedTime) override;
	void OnExit() override;
};
#pragma endregion

#pragma region 回避
class PlayerGreatSwordEvadeState final : public Player8WayHSB
{
public:
	PlayerGreatSwordEvadeState(PlayerStateMachine* stateMachine);
	~PlayerGreatSwordEvadeState() override {}

	// ステート名取得
	const char* GetName() const override { return "CombatEvade"; }
	void OnEnter() override;
	void OnExecute(float elapsedTime) override;
	void OnExit() override {}
};
#pragma endregion

#pragma region 攻撃1
class PlayerGreatSwordAttack1State final : public HierarchicalStateBase<PlayerStateMachine>
{
public:
	PlayerGreatSwordAttack1State(PlayerStateMachine* stateMachine);
	~PlayerGreatSwordAttack1State() override {}

	// ステート名取得
	const char* GetName() const override { return "CombatAttack1"; }
	void OnEnter() override;
	void OnExecute(float elapsedTime) override;
	void OnExit() override {}
private:
	// 先行入力遷移先
	std::string _nextStateName = "";
	// 回転速度
	float _rotationSpeed = 0.1f;
};
#pragma endregion

#pragma region 攻撃2
class PlayerGreatSwordAttack2State final : public HierarchicalStateBase<PlayerStateMachine>
{
public:
	PlayerGreatSwordAttack2State(PlayerStateMachine* stateMachine);
	~PlayerGreatSwordAttack2State() override {}

	// ステート名取得
	const char* GetName() const override { return "CombatAttack2"; }
	void OnEnter() override;
	void OnExecute(float elapsedTime) override;
	void OnExit() override {}
private:
	// 先行入力遷移先
	std::string _nextStateName = "";
};
#pragma endregion

#pragma region ガード
class PlayerGreatSwordGuardState final : public HierarchicalStateBase<PlayerStateMachine>
{
public:
	PlayerGreatSwordGuardState(PlayerStateMachine* stateMachine);
	~PlayerGreatSwordGuardState() override {}

	// ステート名取得
	const char* GetName() const override { return "CombatGuard"; }
	void OnEnter() override;
	void OnExecute(float elapsedTime) override;
	void OnExit() override;
};
#pragma endregion

#pragma region 被弾
class PlayerGreatSwordHitState final : public Player8WayHSB
{
public:
	PlayerGreatSwordHitState(PlayerStateMachine* stateMachine);
	~PlayerGreatSwordHitState() override {}
	// ステート名取得
	const char* GetName() const override { return "CombatHit"; }
	void OnEnter() override;
	void OnExecute(float elapsedTime) override;
	void OnExit() override;
};
class PlayerGreatSwordHitKnockDownState final : public Player8WayHSB
{
public:
	PlayerGreatSwordHitKnockDownState(PlayerStateMachine* stateMachine);
	~PlayerGreatSwordHitKnockDownState() override {}
	// ステート名取得
	const char* GetName() const override { return "CombatHitKnockDown"; }
	void OnEnter() override;
	void OnExecute(float elapsedTime) override;
	void OnExit() override;
};
#pragma endregion

#pragma region 納刀
class PlayerGreatSwordToNonCombatState  final : public PlayerHSB
{
public:
	PlayerGreatSwordToNonCombatState(PlayerStateMachine* stateMachine) :
		PlayerHSB(stateMachine, u8"IdleCombatToIdle", 0.2f, false, true) {
	}
	~PlayerGreatSwordToNonCombatState() override {}
	// ステート名取得
	const char* GetName() const override { return "ToNonCombat"; }
	void OnExecute(float elapsedTime) override;
};
#pragma endregion
