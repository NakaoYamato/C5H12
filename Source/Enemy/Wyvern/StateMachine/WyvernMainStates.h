#pragma once
#include "WyvernStateMachine.h"
#include "../Breath/WyvernBreathActor.h"
#include "../Ball/WyvernBallActor.h"

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

#pragma region 威嚇
class WyvernThreatState : public HierarchicalStateBase<WyvernStateMachine>
{
public:
	WyvernThreatState(WyvernStateMachine* owner) : HierarchicalStateBase(owner) {}
	const char* GetName() const override { return "Threat"; };
	// 開始処理
	void OnEnter() override;
	// 実行処理
	void OnExecute(float elapsedTime) override;
	// 終了処理
	void OnExit() override;
};
#pragma endregion

#pragma region 咆哮
class WyvernRoarState : public HierarchicalStateBase<WyvernStateMachine>
{
public:
	WyvernRoarState(WyvernStateMachine* owner) : HierarchicalStateBase(owner) {}
	const char* GetName() const override { return "Roar"; };
	// 開始処理
	void OnEnter() override;
	// 実行処理
	void OnExecute(float elapsedTime) override;
	// 終了処理
	void OnExit() override;
};
#pragma endregion

#pragma region ターゲットに向く
class WyvernTurnState : public HierarchicalStateBase<WyvernStateMachine>
{
public:
	WyvernTurnState(WyvernStateMachine* owner);
	const char* GetName() const override { return "Turn"; };
	// 開始処理
	void OnEnter() override;
	// 実行処理
	void OnExecute(float elapsedTime) override;
	// 終了処理
	void OnExit() override;
private:
	int _rootNodeIndex = -1;
};
#pragma endregion

#pragma region ターゲットに近づく
class WyvernToTargetState : public HierarchicalStateBase<WyvernStateMachine>
{
public:
	WyvernToTargetState(WyvernStateMachine* owner) :
		HierarchicalStateBase(owner) {
	}
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
private:
	static constexpr float TargetLocalLimitPositionX = 70.0f;
	static constexpr float TargetLocalLimitPositionZ = 330.0f;
	static constexpr float TargetLocalLimitRadius = 90.0f;
	static constexpr float LerpStartAnimationTime	= 1.2f;
	static constexpr float LerpEndAnimationTime		= 1.7f;

	// ターゲット位置
	Vector3 _targetWorldPosition = Vector3(0.0f, 0.0f, 0.0f);
	// 手のノードインデックス
	int _handNodeIndex		= -1;
	float _lerpTimer		= 0.0f;
	float _startLerpSpeed	= 10.0f;
	float _endLerpSpeed		= 15.0f;
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

#pragma region 突進攻撃
class WyvernChargeAttackState : public HierarchicalStateBase<WyvernStateMachine>
{
public:
	WyvernChargeAttackState(WyvernStateMachine* owner) : HierarchicalStateBase(owner) {}
	const char* GetName() const override { return "ChargeAttack"; };
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
	// ブレスのエフェクトを表示するアクター
	std::weak_ptr<WyvernBreathActor> _fireBreathActor;
};
#pragma endregion

#pragma region 火球
class WyvernFireBallAttackState : public HierarchicalStateBase<WyvernStateMachine>
{
public:
	WyvernFireBallAttackState(WyvernStateMachine* owner) : HierarchicalStateBase(owner) {}
	const char* GetName() const override { return "BallAttack"; };
	// 開始処理
	void OnEnter() override;
	// 実行処理
	void OnExecute(float elapsedTime) override;
	// 終了処理
	void OnExit() override;
private:
	// 火球のエフェクトを表示するアクター
	std::weak_ptr<WyvernBallActor> _fireBallActor;
};
#pragma endregion

#pragma region バックジャンプ火球
class WyvernBackJumpFireBallAttackState : public HierarchicalStateBase<WyvernStateMachine>
{
public:
	WyvernBackJumpFireBallAttackState(WyvernStateMachine* owner) : HierarchicalStateBase(owner) {}
	const char* GetName() const override { return "BackJumpBallAttack"; };
	// 開始処理
	void OnEnter() override;
	// 実行処理
	void OnExecute(float elapsedTime) override;
	// 終了処理
	void OnExit() override;
private:
	// 火球のエフェクトを表示するアクター
	std::weak_ptr<WyvernBallActor> _fireBallActor;
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
