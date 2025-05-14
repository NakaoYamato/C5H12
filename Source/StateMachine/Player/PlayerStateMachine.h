#pragma once

#include <variant>
#include "../../Library/Algorithm/StateMachine/StateMachine.h"

// 前方宣言
class PlayerController;
class Animator;

// プレイヤーの状態遷移を管理するクラス
class PlayerStateMachine
{
public:
    PlayerStateMachine(PlayerController* player, Animator* animator);
    ~PlayerStateMachine() {}

	// 実行処理
	void Execute(float elapsedTime);
	// Gui描画
	void DrawGui();

#pragma region アクセサ
	StateMachine<PlayerStateMachine>& GetStateMachine() { return _stateMachine; }
	PlayerController* GetPlayer() { return _player; }
	Animator* GetAnimator() { return _animator; }
#pragma endregion

	// 移動速度
	float _moveSpeed = 10.0f;
	// 攻撃距離
	float _attackRenge = 3.0f;
private:
	StateMachine<PlayerStateMachine> _stateMachine;
	PlayerController* _player = nullptr;
	Animator* _animator = nullptr;
};

#pragma region 各ステート
#pragma region 待機
class PlayerIdleState final : public HierarchicalStateBase<PlayerStateMachine>
{
public:
	PlayerIdleState(PlayerStateMachine* stateMachine) : HierarchicalStateBase(stateMachine) {}
	~PlayerIdleState() override {}

	// ステート名取得
	const char* GetName() const override { return "Idle"; }
	void OnEnter() override;
	void OnExecute(float elapsedTime) override;
	void OnExit() override {}
};
#pragma endregion

#pragma region 攻撃1
class PlayerAttack1State final : public HierarchicalStateBase<PlayerStateMachine>
{
public:
	PlayerAttack1State(PlayerStateMachine* stateMachine);
	~PlayerAttack1State() override {}

	// ステート名取得
	const char* GetName() const override { return "Attack1"; }
	void OnEnter() override;
	void OnExecute(float elapsedTime) override;
	void OnExit() override {}
};
#pragma endregion


#pragma endregion
