#pragma once

#include "../PlayerStateController.h"

// 納刀状態のプレイヤーステート
#pragma region 待機
class PlayerNonCombatIdleState final : public PlayerAnimationHSB
{
public:
	PlayerNonCombatIdleState(PlayerStateController* owner) :
		PlayerAnimationHSB(owner, u8"Idle", 0.2f, true, false) {
	}
	~PlayerNonCombatIdleState() override {}
	// ステート名取得
	const char* GetName() const override { return "Idle"; }
	void OnExecute(float elapsedTime) override;
};
#pragma endregion

#pragma region 振り向き
class PlayerNonCombatTurnState final : public PlayerHSBBase
{
public:
	PlayerNonCombatTurnState(PlayerStateController* owner);
	~PlayerNonCombatTurnState() override {}
	// ステート名取得
	const char* GetName() const override { return "Turn"; }
	void OnEnter() override;
	void OnExecute(float elapsedTime) override;
	void OnExit() override;
};
#pragma endregion

#pragma region 歩き
class PlayerNonCombatWalkState final : public PlayerHSBBase
{
public:
	PlayerNonCombatWalkState(PlayerStateController* owner);
	~PlayerNonCombatWalkState() override {}
	// ステート名取得
	const char* GetName() const override { return "Walk"; }
	void OnEnter() override;
	void OnExecute(float elapsedTime) override;
	void OnExit() override;
};
#pragma endregion

#pragma region 走り
class PlayerNonCombatRunState final : public PlayerHSBBase
{
public:
	PlayerNonCombatRunState(PlayerStateController* owner);
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
	PlayerNonCombatEvadeState(PlayerStateController* owner);
	~PlayerNonCombatEvadeState() override {}

	// ステート名取得
	const char* GetName() const override { return "Evade"; }
	void OnEnter() override;
	void OnExecute(float elapsedTime) override;
	void OnExit() override;
};
#pragma endregion

#pragma region 抜刀
class PlayerNonCombatToCombatState final : public PlayerAnimationHSB
{
public:
	PlayerNonCombatToCombatState(PlayerStateController* owner) :
		PlayerAnimationHSB(owner, u8"IdleToIdleCombat", 0.2f, false, true) {
	}
	~PlayerNonCombatToCombatState() override {}
	// ステート名取得
	const char* GetName() const override { return "ToCombat"; }
	void OnExecute(float elapsedTime) override;
private:
	float _waitAttackTime = 0.1f;
};
#pragma endregion

#pragma region 被弾
class PlayerNonCombatHitState final : public Player8WayHSB
{
public:
	PlayerNonCombatHitState(PlayerStateController* owner);
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
	PlayerNonCombatHitKnockDownState(PlayerStateController* owner);
	~PlayerNonCombatHitKnockDownState() override {}
	// ステート名取得
	const char* GetName() const override { return "HitKnockDown"; }
	void OnEnter() override;
	void OnExecute(float elapsedTime) override;
	void OnExit() override;
};
#pragma endregion

#pragma region ダウン
class PlayerNonCombatDownState final : public PlayerHSBBase
{
public:
	PlayerNonCombatDownState(PlayerStateController* owner);
	~PlayerNonCombatDownState() override {}
	// ステート名取得
	const char* GetName() const override { return "Down"; }
	void OnEnter() override;
	void OnExecute(float elapsedTime) override {}
	void OnExit() override;
};
#pragma endregion

#pragma region 死亡
class PlayerNonCombatDeathState final : public PlayerAnimationHSB
{
public:
	PlayerNonCombatDeathState(PlayerStateController* owner) :
		PlayerAnimationHSB(owner, u8"HitCombatDeath", 0.0f, false, true)
	{
	}
	~PlayerNonCombatDeathState() override {}
	// ステート名取得
	const char* GetName() const override { return "Death"; }
};
#pragma endregion

#pragma region 飲む
class PlayerNonCombatDrinkState final : public PlayerHSBBase
{
public:
	PlayerNonCombatDrinkState(PlayerStateController* owner);
	~PlayerNonCombatDrinkState() override {}
	// ステート名取得
	const char* GetName() const override { return "Drink"; }
	void OnEnter() override;
	void OnExecute(float elapsedTime) override;
	void OnExit() override;
};
#pragma endregion

#pragma region 疲労
class PlayerNonCombatFatigueState final : public PlayerHSBBase
{
public:
	PlayerNonCombatFatigueState(PlayerStateController* owner);
	~PlayerNonCombatFatigueState() override {}
	// ステート名取得
	const char* GetName() const override { return "Fatigue"; }
	void OnEnter() override;
	void OnExecute(float elapsedTime) override;
	void OnExit() override;
private:
	float _recoverStaminaSpeed = 30.0f;
};
#pragma endregion

#pragma region 砥石
class PlayerNonCombatGrindState final : public PlayerHSBBase
{
public:
	PlayerNonCombatGrindState(PlayerStateController* owner);
	~PlayerNonCombatGrindState() override {}
	// ステート名取得
	const char* GetName() const override { return "Grind"; }
	void OnEnter() override;
	void OnExecute(float elapsedTime) override;
	void OnExit() override;
};
#pragma endregion
