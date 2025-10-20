#pragma once
#include "WyvernStateMachine.h"
#include "../Breath/WyvernBreathActor.h"
#include "../Ball/WyvernBallActor.h"

#pragma region 滞空待機
class WyvernHoverIdleState : public HierarchicalStateBase<WyvernStateMachine>
{
public:
	WyvernHoverIdleState(WyvernStateMachine* owner) : HierarchicalStateBase(owner) {}
	const char* GetName() const override { return "HoverIdle"; };
	// 開始処理
	void OnEnter() override;
	// 実行処理
	void OnExecute(float elapsedTime) override;
	// 終了処理
	void OnExit() override;
};
#pragma endregion

#pragma region 空中移動
class WyvernHoverToTargetState : public HierarchicalStateBase<WyvernStateMachine>
{
public:
	WyvernHoverToTargetState(WyvernStateMachine* owner) : HierarchicalStateBase(owner) {}
	const char* GetName() const override { return "HoverToTarget"; };
	// 開始処理
	void OnEnter() override;
	// 実行処理
	void OnExecute(float elapsedTime) override;
	// 終了処理
	void OnExit() override;
};
#pragma endregion

#pragma region 空中回転
class WyvernHoverTurnState : public HierarchicalStateBase<WyvernStateMachine>
{
public:
	WyvernHoverTurnState(WyvernStateMachine* owner);
	const char* GetName() const override { return "HoverTurn"; };
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

#pragma region 空中火球
class WyvernHoverFireBallAttackState : public HierarchicalStateBase<WyvernStateMachine>
{
public:
	WyvernHoverFireBallAttackState(WyvernStateMachine* owner) : HierarchicalStateBase(owner) {}
	const char* GetName() const override { return "HoverFireBallAttack"; };
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

#pragma region 空中かぎ爪攻撃
class WyvernHoverClawAttackState : public HierarchicalStateBase<WyvernStateMachine>
{
public:
	WyvernHoverClawAttackState(WyvernStateMachine* owner) : HierarchicalStateBase(owner) {}
	const char* GetName() const override { return "HoverClawAttack"; };
	// 開始処理
	void OnEnter() override;
	// 実行処理
	void OnExecute(float elapsedTime) override;
	// 終了処理
	void OnExit() override;
};
#pragma endregion

#pragma region 墜落
class WyvernHitFallState : public HierarchicalStateBase<WyvernStateMachine>
{
public:
	WyvernHitFallState(WyvernStateMachine* owner) : HierarchicalStateBase(owner) {}
	const char* GetName() const override { return "HitFall"; };
	// 開始処理
	void OnEnter() override;
	// 実行処理
	void OnExecute(float elapsedTime) override;
	// 終了処理
	void OnExit() override;
};
#pragma endregion

#pragma region 着地
class WyvernLandState : public HierarchicalStateBase<WyvernStateMachine>
{
public:
	WyvernLandState(WyvernStateMachine* owner) : HierarchicalStateBase(owner) {}
	const char* GetName() const override { return "Land"; };
	// 開始処理
	void OnEnter() override;
	// 実行処理
	void OnExecute(float elapsedTime) override;
	// 終了処理
	void OnExit() override;
};
#pragma endregion
