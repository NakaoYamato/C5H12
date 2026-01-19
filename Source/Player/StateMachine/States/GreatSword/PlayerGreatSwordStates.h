#pragma once

#include "../../PlayerStateController.h"

// 大剣装備状態のプレイヤーステート
#pragma region 待機
class PlayerGreatSwordIdleState final : public PlayerAnimationHSB
{
public:
	PlayerGreatSwordIdleState(PlayerStateController* owner) : 
		PlayerAnimationHSB(owner, u8"IdleCombat", 0.2f, true, false) {}
	~PlayerGreatSwordIdleState() override {}
	// ステート名取得
	const char* GetName() const override { return "CombatIdle"; }
	void OnExecute(float elapsedTime) override;
};
#pragma endregion

#pragma region 走り
class PlayerGreatSwordRunState final : public PlayerHSBBase
{
public:
	PlayerGreatSwordRunState(PlayerStateController* owner);
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
	PlayerGreatSwordEvadeState(PlayerStateController* owner);
	~PlayerGreatSwordEvadeState() override {}

	// ステート名取得
	const char* GetName() const override { return "CombatEvade"; }
	void OnEnter() override;
	void OnExecute(float elapsedTime) override;
	void OnExit() override;
};
#pragma endregion

#pragma region 攻撃1
class PlayerGreatSwordAttack1State final : public PlayerHSBBase
{
public:
	PlayerGreatSwordAttack1State(PlayerStateController* owner);
	~PlayerGreatSwordAttack1State() override {}

	// ステート名取得
	const char* GetName() const override { return "CombatAttack1"; }
	void OnEnter() override;
	void OnExecute(float elapsedTime) override;
	void OnExit() override {}
private:
	std::string _nextStateName = "";
	// 回転速度
	float _rotationSpeed = 0.07f;
};
#pragma endregion

#pragma region 攻撃2
class PlayerGreatSwordAttack2State final : public PlayerHSBBase
{
public:
	PlayerGreatSwordAttack2State(PlayerStateController* owner);
	~PlayerGreatSwordAttack2State() override {}

	// ステート名取得
	const char* GetName() const override { return "CombatAttack2"; }
	void OnEnter() override;
	void OnExecute(float elapsedTime) override;
	void OnExit() override {}
private:
	std::string _nextStateName = "";
};
#pragma endregion

#pragma region ガード
class PlayerGreatSwordGuardState final : public PlayerHSBBase
{
public:
	PlayerGreatSwordGuardState(PlayerStateController* owner);
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
	PlayerGreatSwordHitState(PlayerStateController* owner);
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
	PlayerGreatSwordHitKnockDownState(PlayerStateController* owner);
	~PlayerGreatSwordHitKnockDownState() override {}
	// ステート名取得
	const char* GetName() const override { return "CombatHitKnockDown"; }
	void OnEnter() override;
	void OnExecute(float elapsedTime) override;
	void OnExit() override;
};
#pragma endregion

#pragma region ダウン
class PlayerGreatSwordDownState final : public PlayerHSBBase
{
public:
	PlayerGreatSwordDownState(PlayerStateController* owner);
	~PlayerGreatSwordDownState() override {}
	// ステート名取得
	const char* GetName() const override { return "CombatDown"; }
	void OnEnter() override;
	void OnExecute(float elapsedTime) override {}
	void OnExit() override;
};
#pragma endregion

#pragma region 納刀
class PlayerGreatSwordToNonCombatState  final : public PlayerAnimationHSB
{
public:
	PlayerGreatSwordToNonCombatState(PlayerStateController* owner) :
		PlayerAnimationHSB(owner, u8"IdleCombatToIdle", 0.2f, false, true) {
	}
	~PlayerGreatSwordToNonCombatState() override {}
	// ステート名取得
	const char* GetName() const override { return "ToNonCombat"; }
	void OnExecute(float elapsedTime) override;
};
#pragma endregion
