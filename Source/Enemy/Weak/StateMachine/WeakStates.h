#pragma once

#include "WeakStateMachine.h"
#include "../WeakActor.h"

#pragma region 待機
class WeakIdleState : public HierarchicalStateBase<WeakStateMachine>
{
public:
	WeakIdleState(WeakStateMachine* owner) : HierarchicalStateBase(owner) {}
	const char* GetName() const override { return "Idle"; }
	// 開始処理
	void OnEnter() override;
	// 実行処理
	void OnExecute(float elapsedTime) override;
	// 終了処理
	void OnExit() override;
};
#pragma endregion
#pragma region 休憩
class WeakRestState : public HierarchicalStateBase<WeakStateMachine>
{
public:
	WeakRestState(WeakStateMachine* owner) : HierarchicalStateBase(owner) {}
	const char* GetName() const override { return "Rest"; }
	// 開始処理
	void OnEnter() override;
	// 実行処理
	void OnExecute(float elapsedTime) override;
	// 終了処理
	void OnExit() override;
};
#pragma endregion
#pragma region 威嚇
class WeakThreatState : public HierarchicalStateBase<WeakStateMachine>
{
public:
	WeakThreatState(WeakStateMachine* owner) : HierarchicalStateBase(owner) {}
	const char* GetName() const override { return "Threat"; }
	// 開始処理
	void OnEnter() override;
	// 実行処理
	void OnExecute(float elapsedTime) override;
	// 終了処理
	void OnExit() override;
};
#pragma endregion
#pragma region 移動
class WeakMoveState : public HierarchicalStateBase<WeakStateMachine>
{
public:
	WeakMoveState(WeakStateMachine* owner) : HierarchicalStateBase(owner) {}
	const char* GetName() const override { return "Move"; }
	// 開始処理
	void OnEnter() override;
	// 実行処理
	void OnExecute(float elapsedTime) override;
	// 終了処理
	void OnExit() override;
};
#pragma endregion
#pragma region ダッシュ
class WeakDashState : public HierarchicalStateBase<WeakStateMachine>
{
public:
	WeakDashState(WeakStateMachine* owner) : HierarchicalStateBase(owner) {}
	const char* GetName() const override { return "Dash"; }
	// 開始処理
	void OnEnter() override;
	// 実行処理
	void OnExecute(float elapsedTime) override;
	// 終了処理
	void OnExit() override;
};
#pragma endregion
#pragma region 攻撃
class WeakAttackState : public HierarchicalStateBase<WeakStateMachine>
{
public:
	WeakAttackState(WeakStateMachine* owner) : HierarchicalStateBase(owner) {}
	const char* GetName() const override { return "Attack"; }
	// 開始処理
	void OnEnter() override;
	// 実行処理
	void OnExecute(float elapsedTime) override;
	// 終了処理
	void OnExit() override;
private:
	// 攻撃後の待機時間
	float _waitTime = 1.0f;
};
#pragma endregion
#pragma region ジャンプ攻撃
class WeakJumpAttackState : public HierarchicalStateBase<WeakStateMachine>
{
public:
	WeakJumpAttackState(WeakStateMachine* owner) : HierarchicalStateBase(owner) {}
	const char* GetName() const override { return "JumpAttack"; }
	// 開始処理
	void OnEnter() override;
	// 実行処理
	void OnExecute(float elapsedTime) override;
	// 終了処理
	void OnExit() override;
private:
	// 攻撃後の待機時間
	float _waitTime = 1.5f;
};
#pragma endregion
#pragma region 被弾
class WeakDamageState : public HierarchicalStateBase<WeakStateMachine>
{
public:
	WeakDamageState(WeakStateMachine* owner) : HierarchicalStateBase(owner) {}
	const char* GetName() const override { return "Damage"; }
	// 開始処理
	void OnEnter() override;
	// 実行処理
	void OnExecute(float elapsedTime) override;
	// 終了処理
	void OnExit() override;
};
#pragma endregion
#pragma region 死亡
class WeakDeathState : public HierarchicalStateBase<WeakStateMachine>
{
public:
	WeakDeathState(WeakStateMachine* owner) : HierarchicalStateBase(owner) {}
	const char* GetName() const override { return "Death"; }
	// 開始処理
	void OnEnter() override;
	// 実行処理
	void OnExecute(float elapsedTime) override;
	// 終了処理
	void OnExit() override;
};
#pragma endregion
