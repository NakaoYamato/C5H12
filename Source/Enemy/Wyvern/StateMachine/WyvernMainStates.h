#pragma once
#include "WyvernStateMachine.h"
#include "../Breath/WyvernBreathActor.h"

#pragma region 待機
class WyvernIdleState : public HierarchicalStateBase<WyvernStateMachine>
{
public:
	WyvernIdleState(WyvernStateMachine* owner) : HierarchicalStateBase(owner) {}
	const char* GetName() const override { return "Idle"; };
	// 開始処理
	void OnEnter() override;
	// 実行処理
	void OnExecute(float elapsedTime) override;
	// 終了処理
	void OnExit() override;
};
#pragma endregion

#pragma region ターゲットに向かう
class WyvernToTargetState : public HierarchicalStateBase<WyvernStateMachine>
{
public:
	WyvernToTargetState(WyvernStateMachine* owner);
	const char* GetName() const override { return "ToTarget"; };
	// 開始処理
	void OnEnter() override;
	// 実行処理
	void OnExecute(float elapsedTime) override;
	// 終了処理
	void OnExit() override;
};
#pragma endregion

#pragma region 噛みつき攻撃
class WyvernBiteAttackState : public HierarchicalStateBase<WyvernStateMachine>
{
public:
	WyvernBiteAttackState(WyvernStateMachine* owner) : HierarchicalStateBase(owner) {}
	const char* GetName() const override { return "BiteAttack"; };
	// 開始処理
	void OnEnter() override;
	// 実行処理
	void OnExecute(float elapsedTime) override;
	// 終了処理
	void OnExit() override;
};
#pragma endregion

#pragma region かぎ爪攻撃
class WyvernClawAttackState : public HierarchicalStateBase<WyvernStateMachine>
{
public:
	WyvernClawAttackState(WyvernStateMachine* owner) : HierarchicalStateBase(owner) {}
	const char* GetName() const override { return "ClawAttack"; };
	// 開始処理
	void OnEnter() override;
	// 実行処理
	void OnExecute(float elapsedTime) override;
	// 終了処理
	void OnExit() override;
};
#pragma endregion

#pragma region 尻尾攻撃
class WyvernTailAttackState : public HierarchicalStateBase<WyvernStateMachine>
{
public:
	WyvernTailAttackState(WyvernStateMachine* owner) : HierarchicalStateBase(owner) {}
	const char* GetName() const override { return "TailAttack"; };
	// 開始処理
	void OnEnter() override;
	// 実行処理
	void OnExecute(float elapsedTime) override;
	// 終了処理
	void OnExit() override;
};
#pragma endregion

#pragma region 後退
class WyvernBackStepState : public HierarchicalStateBase<WyvernStateMachine>
{
public:
	WyvernBackStepState(WyvernStateMachine* owner) : HierarchicalStateBase(owner) {}
	const char* GetName() const override { return "BackStep"; };
	// 開始処理
	void OnEnter() override;
	// 実行処理
	void OnExecute(float elapsedTime) override;
	// 終了処理
	void OnExit() override;
};
#pragma endregion

#pragma region ブレス
class WyvernBreathAttackState : public HierarchicalStateBase<WyvernStateMachine>
{
public:
	WyvernBreathAttackState(WyvernStateMachine* owner) : HierarchicalStateBase(owner) {}
	const char* GetName() const override { return "BreathAttack"; };
	// 開始処理
	void OnEnter() override;
	// 実行処理
	void OnExecute(float elapsedTime) override;
	// 終了処理
	void OnExit() override;

private:
	std::weak_ptr<WyvernBreathActor> _fireBreathActor; // ブレスのエフェクトを表示するアクター
	Vector3 _fireBreathOffset = Vector3(-19.0f, 100.0f, 0.0f); // ブレスの位置
};
#pragma endregion

#pragma region 近づく
class WyvernPursuitState : public HierarchicalStateBase<WyvernStateMachine>
{
public:
	WyvernPursuitState(WyvernStateMachine* owner) : HierarchicalStateBase(owner) {}
	const char* GetName() const override { return "Pursuit"; };
	// 開始処理
	void OnEnter() override;
	// 実行処理
	void OnExecute(float elapsedTime) override;
	// 終了処理
	void OnExit() override;
};
#pragma endregion

#pragma region ダメージを受ける
class WyvernDamageState : public HierarchicalStateBase<WyvernStateMachine>
{
public:
	WyvernDamageState(WyvernStateMachine* owner) : HierarchicalStateBase(owner) {}
	const char* GetName() const override { return "Damage"; };
	// 開始処理
	void OnEnter() override;
	// 実行処理
	void OnExecute(float elapsedTime) override;
	// 終了処理
	void OnExit() override;
private:
	// 前方向と判定する角度(cos値)
	const float _frontAngleThreshold = 0.8f;
	// 後方向と判定する角度(cos値)
	const float _backAngleThreshold = -0.3f;

	// アニメーションの回転を適用するかどうか
	bool _applyRotation = false;
};
#pragma endregion

#pragma region 死亡
class WyvernDeathState : public HierarchicalStateBase<WyvernStateMachine>
{
public:
	WyvernDeathState(WyvernStateMachine* owner) : HierarchicalStateBase(owner) {}
	const char* GetName() const override { return "Death"; };
	// 開始処理
	void OnEnter() override;
	// 実行処理
	void OnExecute(float elapsedTime) override;
	// 終了処理
	void OnExit() override;
};
#pragma endregion
