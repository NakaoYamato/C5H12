#pragma once

#include <variant>
#include "../../Library/Component/StateController.h"
#include "../../Library/Math/Vector.h"

// 前方宣言
class PlayerController;
class Animator;

// プレイヤーの状態遷移を管理するクラス
class PlayerStateMachine : public StateMachine
{
public:
    PlayerStateMachine(PlayerController* player, Animator* animator);
    ~PlayerStateMachine() {}

	// 開始処理
	void Start();
	// 実行処理
	void Execute(float elapsedTime) override;
	// Gui描画
	void DrawGui() override;

	/// <summary>
	/// 移動方向に向く
	/// </summary>
	/// <param name="elapsedTime">経過時間</param>
	/// <param name="rotationSpeed">回転速度</param>
	void RotationMovement(float elapsedTime, float rotationSpeed = 1.0f);
#pragma region アクセサ
	StateMachineBase<PlayerStateMachine>& GetStateMachine() { return _stateMachine; }
	PlayerController* GetPlayer() { return _player; }
	Animator* GetAnimator() { return _animator; }
	// ステート変更
	void ChangeState(const char* mainStateName, const char* subStateName) override;
    // ステート名取得
	const char* GetStateName() override;
    // サブステート名取得
	const char* GetSubStateName() override;
	// キャンセルイベントを取得
	bool CallCancelEvent() const { return _callCancelEvent; }
	const Vector2& GetMovement() const { return _movement; }
	bool IsMoving()	const { return _isMoving; }
	bool IsDash()	const { return _isDash; }
	bool IsEvade()	const { return _isEvade; }
	bool IsAttack() const { return _isAttack; }
	bool IsGuard()	const { return _isGuard; }
	bool IsDead()	const { return _isDead; }

	void SetMovement(const Vector2& movement)	{ _movement = movement; }
	void SetIsMoving(bool isMoving)				{ _isMoving = isMoving; }
	void SetIsDash(bool isDush)					{ _isDash = isDush; }
	void SetIsEvade(bool isEvade)				{ _isEvade = isEvade; }
	void SetIsAttack(bool isAttack)				{ _isAttack = isAttack; }
	void SetIsGuard(bool isGuard)				{ _isGuard = isGuard; }
	void SetIsDead(bool isDead)					{ _isDead = isDead; }
#pragma endregion
private:
	StateMachineBase<PlayerStateMachine> _stateMachine;
	PlayerController* _player = nullptr;
	Animator* _animator = nullptr;

#pragma region 各種フラグ
	bool _callCancelEvent = false;
	bool _callInvisivleEvent = false;
	// 入力方向をワールド空間に変換したもの
	Vector2 _movement	= { 0.0f, 0.0f };
	bool _isMoving		= false;
	bool _isDash		= false;
	bool _isEvade		= false;
	bool _isAttack		= false;
	bool _isGuard		= false;
	bool _isDead		= false;
#pragma endregion

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

#pragma region 走り
class PlayerRunState final : public HierarchicalStateBase<PlayerStateMachine>
{
public:
	PlayerRunState(PlayerStateMachine* stateMachine) : HierarchicalStateBase(stateMachine) {}
	~PlayerRunState() override {}

	// ステート名取得
	const char* GetName() const override { return "Run"; }
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
	const char* GetName() const override { return "Sprint"; }
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
	const char* GetName() const override { return "Evade"; }
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

#pragma region ガード
class PlayerGuardState final : public HierarchicalStateBase<PlayerStateMachine>
{
public:
	PlayerGuardState(PlayerStateMachine* stateMachine);
	~PlayerGuardState() override {}

	// ステート名取得
	const char* GetName() const override { return "Guard"; }
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
	const char* GetName() const override { return "Hit"; }
	void OnEnter() override;
	void OnExecute(float elapsedTime) override;
	void OnExit() override;
};
class PlayerHitKnockDownState final : public HierarchicalStateBase<PlayerStateMachine>
{
public:
	PlayerHitKnockDownState(PlayerStateMachine* stateMachine) : HierarchicalStateBase(stateMachine) {}
	~PlayerHitKnockDownState() override {}
	// ステート名取得
	const char* GetName() const override { return "HitKnockDown"; }
	void OnEnter() override;
	void OnExecute(float elapsedTime) override;
	void OnExit() override;
};
class PlayerGuardHitState final : public HierarchicalStateBase<PlayerStateMachine>
{
public:
	PlayerGuardHitState(PlayerStateMachine* stateMachine) : HierarchicalStateBase(stateMachine) {}
	~PlayerGuardHitState() override {}
	// ステート名取得
	const char* GetName() const override { return "GuardHit"; }
	void OnEnter() override;
	void OnExecute(float elapsedTime) override;
	void OnExit() override;
};
class PlayerGuardBreakState final : public HierarchicalStateBase<PlayerStateMachine>
{
public:
	PlayerGuardBreakState(PlayerStateMachine* stateMachine) : HierarchicalStateBase(stateMachine) {}
	~PlayerGuardBreakState() override {}
	// ステート名取得
	const char* GetName() const override { return "GuardBreak"; }
	void OnEnter() override;
	void OnExecute(float elapsedTime) override;
	void OnExit() override;
};
#pragma endregion

#pragma region 死亡
class PlayerDeathState final : public HierarchicalStateBase<PlayerStateMachine>
{
public:
	PlayerDeathState(PlayerStateMachine* stateMachine) : HierarchicalStateBase(stateMachine) {}
	~PlayerDeathState() override {}
	// ステート名取得
	const char* GetName() const override { return "Death"; }
	void OnEnter() override;
	void OnExecute(float elapsedTime) override;
	void OnExit() override {}
};
#pragma endregion


#pragma endregion
