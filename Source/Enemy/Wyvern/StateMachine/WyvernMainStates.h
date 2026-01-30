#pragma once
#include "WyvernStateController.h"
#include "../Breath/WyvernBreathActor.h"
#include "../Ball/WyvernBallActor.h"

#pragma region 待機
class WyvernIdleState final : public WyvernHSB
{
public:
	WyvernIdleState(WyvernStateController* owner) :
		WyvernHSB(
			owner,
			u8"Idle03Shake",
			1.5f,
			false,	// isLoop
			false,	// isUsingRootMotion
			true	// applyRotation
			) {}
	const char* GetName() const override { return "Idle"; };
	// 実行処理
	void OnExecute(float elapsedTime) override;
};
#pragma endregion

#pragma region 威嚇
class WyvernThreatState final : public WyvernHSB
{
public:
	WyvernThreatState(WyvernStateController* owner) :
		WyvernHSB(
			owner,
			u8"IdleCombat",
			1.5f,
			false,	// isLoop
			false,	// isUsingRootMotion
			true	// applyRotation
		) {}
	const char* GetName() const override { return "Threat"; };
	// 実行処理
	void OnExecute(float elapsedTime) override;
};
#pragma endregion

#pragma region 咆哮
class WyvernRoarState final : public WyvernHSB
{
public:
	WyvernRoarState(WyvernStateController* owner) :
		WyvernHSB(
			owner,
			u8"Roar",
			1.5f,
			false,	// isLoop
			false,	// isUsingRootMotion
			true	// applyRotation
		) {
	}
	const char* GetName() const override { return "Roar"; };
	// 実行処理
	void OnExecute(float elapsedTime) override;
};
#pragma endregion

#pragma region ターゲットに向く
class WyvernTurnState final : public HierarchicalStateBase<WyvernStateController>
{
public:
	WyvernTurnState(WyvernStateController* owner);
	const char* GetName() const override { return "Turn"; };
	// 開始処理
	void OnEnter() override;
	// 実行処理
	void OnExecute(float elapsedTime) override;
	// 終了処理
	void OnExit() override;
};
#pragma endregion

#pragma region ターゲットに近づく
class WyvernToTargetState final : public WyvernHSB
{
public:
	WyvernToTargetState(WyvernStateController* owner) :
		WyvernHSB(
			owner,
			u8"WalkForward",
			3.0f,
			false,	// isLoop
			true,	// isUsingRootMotion
			true	// applyRotation
		) {
	}
	const char* GetName() const override { return "ToTarget"; };
	// 実行処理
	void OnExecute(float elapsedTime) override;
};
#pragma endregion

#pragma region 噛みつき攻撃
class WyvernBiteAttackState final : public HierarchicalStateBase<WyvernStateController>
{
public:
	WyvernBiteAttackState(WyvernStateController* owner);
	const char* GetName() const override { return "BiteAttack"; };
	// 開始処理
	void OnEnter() override;
	// 実行処理
	void OnExecute(float elapsedTime) override;
	// 終了処理
	void OnExit() override;
private:
    static constexpr float MotionAttackFactor = 1.0f;
};
#pragma endregion

#pragma region かぎ爪攻撃
class WyvernClawAttackState final : public HierarchicalStateBase<WyvernStateController>
{
public:
	WyvernClawAttackState(WyvernStateController* owner);
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
private:
	static constexpr float MotionAttackFactor = 2.0f;
};
#pragma endregion

#pragma region 尻尾攻撃
class WyvernTailAttackState final : public HierarchicalStateBase<WyvernStateController>
{
public:
	WyvernTailAttackState(WyvernStateController* owner);
	const char* GetName() const override { return "TailAttack"; };
	// 開始処理
	void OnEnter() override;
	// 実行処理
	void OnExecute(float elapsedTime) override;
	// 終了処理
	void OnExit() override;
private:
	static constexpr float MotionAttackFactor = 5.0f;
};
#pragma endregion

#pragma region 突進攻撃
class WyvernChargeAttackState final : public WyvernHSB
{
public:
	WyvernChargeAttackState(WyvernStateController* owner) :
		WyvernHSB(
			owner,
			u8"AttackCharge",
			3.5f,
			false,	// isLoop
			true,	// isUsingRootMotion
			true	// applyRotation
		) {
	}
	const char* GetName() const override { return "ChargeAttack"; };
	// 開始処理
	void OnEnter() override;
	// 実行処理
	void OnExecute(float elapsedTime) override;
	// 終了処理
	void OnExit() override;
private:
	Vector3 _startPosition = Vector3::Zero;
private:
	static constexpr float MotionAttackFactor = 5.0f;
};
#pragma endregion

#pragma region 後退
class WyvernBackStepState final : public WyvernHSB
{
public:
	WyvernBackStepState(WyvernStateController* owner) :
		WyvernHSB(
			owner,
			u8"WalkBack",
			2.5f,
			false,	// isLoop
			true,	// isUsingRootMotion
			true	// applyRotation
		) {
	}
	const char* GetName() const override { return "BackStep"; };
	// 実行処理
	void OnExecute(float elapsedTime) override;
};
#pragma endregion

#pragma region ブレス
class WyvernBreathAttackState final : public WyvernHSB
{
public:
	WyvernBreathAttackState(WyvernStateController* owner) :
		WyvernHSB(
			owner,
			u8"AttackFireBreath",
			1.5f,
			false,	// isLoop
			false,	// isUsingRootMotion
			true	// applyRotation
		) {
	}
	const char* GetName() const override { return "BreathAttack"; };
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
class WyvernFireBallAttackState final : public WyvernHSB
{
public:
	WyvernFireBallAttackState(WyvernStateController* owner) :
		WyvernHSB(
			owner,
			u8"AttackFireBall",
			1.5f,
			false,	// isLoop
			false,	// isUsingRootMotion
			true	// applyRotation
		) {
	}
	const char* GetName() const override { return "BallAttack"; };
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
class WyvernBackJumpFireBallAttackState final : public WyvernHSB
{
public:
	WyvernBackJumpFireBallAttackState(WyvernStateController* owner) :
		WyvernHSB(
			owner,
			u8"AttackBackJumpBall",
			1.5f,
			false,	// isLoop
			true,	// isUsingRootMotion
			true	// applyRotation
		) {
	}
	const char* GetName() const override { return "BackJumpBallAttack"; };
	// 実行処理
	void OnExecute(float elapsedTime) override;
	// 終了処理
	void OnExit() override;
private:
	// 火球のエフェクトを表示するアクター
	std::weak_ptr<WyvernBallActor> _fireBallActor;
	// 発射角度
	const float _launchAngleX = DirectX::XMConvertToRadians(50.0f);
};
#pragma endregion

#pragma region 近づく
class WyvernPursuitState : public HierarchicalStateBase<WyvernStateController>
{
public:
	WyvernPursuitState(WyvernStateController* owner) : HierarchicalStateBase(owner) {}
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
class WyvernDamageState : public HierarchicalStateBase<WyvernStateController>
{
public:
	WyvernDamageState(WyvernStateController* owner);
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
};
#pragma endregion

#pragma region ダウン
class WyvernDownState : public HierarchicalStateBase<WyvernStateController>
{
public:
	WyvernDownState(WyvernStateController* owner);
	const char* GetName() const override { return "Down"; };
	// 開始処理
	void OnEnter() override;
	// 終了処理
	void OnExit() override;
private:
	float _downTime = 5.0f;
};
#pragma endregion

#pragma region 死亡
class WyvernDeathState : public HierarchicalStateBase<WyvernStateController>
{
public:
	WyvernDeathState(WyvernStateController* owner) : HierarchicalStateBase(owner) {}
	const char* GetName() const override { return "Death"; };
	// 開始処理
	void OnEnter() override;
	// 実行処理
	void OnExecute(float elapsedTime) override;
	// 終了処理
	void OnExit() override;
};
#pragma endregion

#pragma region 位置調整
class WyvernPositionAdjustState : public HierarchicalStateBase<WyvernStateController>
{
public:
	WyvernPositionAdjustState(WyvernStateController* owner);
	const char* GetName() const override { return "PositionAdjust"; };
	// 開始処理
	void OnEnter() override;
	// 実行処理
	void OnExecute(float elapsedTime) override;
	// 終了処理
	void OnExit() override;
};
#pragma endregion
