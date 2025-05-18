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

	// ステート変更
	void ChangeState(enum class PlayerMainStates mainStateName, enum class PlayerSubStates subStateName);
    // ステート名取得
    const char* GetStateName() { return _stateMachine.GetState()->GetName(); }
    // サブステート名取得
    const char* GetSubStateName() { return _stateMachine.GetState()->GetSubStateName(); }

	// キャンセルイベントを取得
	bool CallCancelEvent() const { return _callCancelEvent; }
#pragma endregion
private:
	StateMachine<PlayerStateMachine> _stateMachine;
	PlayerController* _player = nullptr;
	Animator* _animator = nullptr;

	bool _callCancelEvent = false;
};

#pragma region 各ステート
#pragma region 待機
class PlayerIdleState final : public HierarchicalStateBase<PlayerStateMachine>
{
public:
	PlayerIdleState(PlayerStateMachine* stateMachine) : HierarchicalStateBase(stateMachine) {}
	~PlayerIdleState() override {}

	// ステート名取得
	const char* GetName() const override;
	void OnEnter() override;
	void OnExecute(float elapsedTime) override;
	void OnExit() override {}
};
#pragma endregion

#pragma region 走り
class PlayerRunState final : public HierarchicalStateBase<PlayerStateMachine>
{
public:
	PlayerRunState(PlayerStateMachine* stateMachine) : HierarchicalStateBase(stateMachine) {}
	~PlayerRunState() override {}

	// ステート名取得
	const char* GetName() const override;
	void OnEnter() override;
	void OnExecute(float elapsedTime) override;
	void OnExit() override;
};
#pragma endregion

#pragma region ダッシュ
class PlayerSprintState final : public HierarchicalStateBase<PlayerStateMachine>
{
public:
	PlayerSprintState(PlayerStateMachine* stateMachine);
	~PlayerSprintState() override {}

	// ステート名取得
	const char* GetName() const override;
	void OnEnter() override;
	void OnExecute(float elapsedTime) override;
	void OnExit() override;
};
#pragma endregion

#pragma region 回避
class PlayerEvadeState final : public HierarchicalStateBase<PlayerStateMachine>
{
public:
	PlayerEvadeState(PlayerStateMachine* stateMachine) : HierarchicalStateBase(stateMachine) {}
	~PlayerEvadeState() override {}

	// ステート名取得
	const char* GetName() const override;
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
	const char* GetName() const override;
	void OnEnter() override;
	void OnExecute(float elapsedTime) override;
	void OnExit() override {}
};
#pragma endregion

#pragma region ガード
class PlayerGuardState final : public HierarchicalStateBase<PlayerStateMachine>
{
public:
	PlayerGuardState(PlayerStateMachine* stateMachine);
	~PlayerGuardState() override {}

	// ステート名取得
	const char* GetName() const override;
	void OnEnter() override;
	void OnExecute(float elapsedTime) override;
	void OnExit() override;
};
#pragma endregion

#pragma region 被弾
class PlayerHitState final : public HierarchicalStateBase<PlayerStateMachine>
{
public:
	PlayerHitState(PlayerStateMachine* stateMachine) : HierarchicalStateBase(stateMachine) {}
	~PlayerHitState() override {}
	// ステート名取得
	const char* GetName() const override;
	void OnEnter() override;
	void OnExecute(float elapsedTime) override;
	void OnExit() override {}
};
class PlayerHitKnockDownState final : public HierarchicalStateBase<PlayerStateMachine>
{
public:
	PlayerHitKnockDownState(PlayerStateMachine* stateMachine) : HierarchicalStateBase(stateMachine) {}
	~PlayerHitKnockDownState() override {}
	// ステート名取得
	const char* GetName() const override;
	void OnEnter() override;
	void OnExecute(float elapsedTime) override;
	void OnExit() override {}
};
class PlayerGuardHitState final : public HierarchicalStateBase<PlayerStateMachine>
{
public:
	PlayerGuardHitState(PlayerStateMachine* stateMachine) : HierarchicalStateBase(stateMachine) {}
	~PlayerGuardHitState() override {}
	// ステート名取得
	const char* GetName() const override;
	void OnEnter() override;
	void OnExecute(float elapsedTime) override;
	void OnExit() override {}
};
class PlayerGuardBreakState final : public HierarchicalStateBase<PlayerStateMachine>
{
public:
	PlayerGuardBreakState(PlayerStateMachine* stateMachine) : HierarchicalStateBase(stateMachine) {}
	~PlayerGuardBreakState() override {}
	// ステート名取得
	const char* GetName() const override;
	void OnEnter() override;
	void OnExecute(float elapsedTime) override;
	void OnExit() override {}
};
#pragma endregion

#pragma region 死亡
class PlayerDeathState final : public HierarchicalStateBase<PlayerStateMachine>
{
public:
	PlayerDeathState(PlayerStateMachine* stateMachine) : HierarchicalStateBase(stateMachine) {}
	~PlayerDeathState() override {}
	// ステート名取得
	const char* GetName() const override;
	void OnEnter() override;
	void OnExecute(float elapsedTime) override;
	void OnExit() override {}
};
#pragma endregion


#pragma endregion
