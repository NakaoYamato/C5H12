#pragma once

#include "PlayerStateMachine.h"

// プレイヤーのヒエラルキカルステートのベースクラス
#pragma region ベースステート
// OnEnterでアニメーションを再生するだけの簡易ステート
class PlayerHSB : public HierarchicalStateBase<PlayerStateMachine>
{
public:
	PlayerHSB(PlayerStateMachine* stateMachine,
		const std::string& animationName,
		float blendSeconds,
		bool isLoop,
		bool isUsingRootMotion) :
		HierarchicalStateBase(stateMachine),
		_animationName(animationName),
		_blendSeconds(blendSeconds),
		_isLoop(isLoop),
		_isUsingRootMotion(isUsingRootMotion)
	{
	}
	~PlayerHSB() override {}
	virtual void OnEnter() override;
	virtual void OnExecute(float elapsedTime) override {}
	virtual void OnExit() override {}
private:
	std::string _animationName = "";
	float		_blendSeconds = 0.2f;
	bool		_isLoop = false;
	bool		_isUsingRootMotion = false;
};

// 8方向のサブステートを持つヒエラルキカルステート
class Player8WayHSB : public HierarchicalStateBase<PlayerStateMachine>
{
public:
	enum Direction
	{
		Front = 0,
		FrontRight,
		Right,
		BackRight,
		Back,
		BackLeft,
		Left,
		FrontLeft,

		NumDirections,
	};

public:
	Player8WayHSB(PlayerStateMachine* stateMachine,
		std::vector<std::string> animationNames,
		float blendSeconds,
		bool isUsingRootMotion);
	~Player8WayHSB() override {}
	virtual void OnEnter() override {}
	virtual void OnExecute(float elapsedTime) override {}
	virtual void OnExit() override {}
	void ChangeSubState(Direction animationIndex);
};
#pragma endregion

// 大剣装備状態のプレイヤーステート
#pragma region 待機
class PlayerGreatSwordIdleState final : public PlayerHSB
{
public:
	PlayerGreatSwordIdleState(PlayerStateMachine* stateMachine) : 
		PlayerHSB(stateMachine, u8"IdleCombat", 0.2f, true, false) {}
	~PlayerGreatSwordIdleState() override {}
	// ステート名取得
	const char* GetName() const override { return "GreatSwordIdle"; }
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
	const char* GetName() const override { return "GreatSwordWalk"; }
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
	const char* GetName() const override { return "GreatSwordEvade"; }
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
	const char* GetName() const override { return "GreatSwordAttack1"; }
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
	const char* GetName() const override { return "GreatSwordGuard"; }
	void OnEnter() override;
	void OnExecute(float elapsedTime) override;
	void OnExit() override;
};
#pragma endregion
