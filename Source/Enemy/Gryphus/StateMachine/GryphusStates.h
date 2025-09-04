#pragma once

#include "GryphusStateMachine.h"
#include "../GryphusActor.h"

#pragma region 待機
class GryphusIdleState : public HierarchicalStateBase<GryphusStateMachine>
{
public:
    GryphusIdleState(GryphusStateMachine* owner) : HierarchicalStateBase(owner) {}
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
class GryphusRestState : public HierarchicalStateBase<GryphusStateMachine>
{
public:
    GryphusRestState(GryphusStateMachine* owner) : HierarchicalStateBase(owner) {}
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
class GryphusThreatState : public HierarchicalStateBase<GryphusStateMachine>
{
public:
    GryphusThreatState(GryphusStateMachine* owner) : HierarchicalStateBase(owner) {}
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
class GryphusMoveState : public HierarchicalStateBase<GryphusStateMachine>
{
public:
    GryphusMoveState(GryphusStateMachine* owner) : HierarchicalStateBase(owner) {}
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
class GryphusDashState : public HierarchicalStateBase<GryphusStateMachine>
{
public:
    GryphusDashState(GryphusStateMachine* owner) : HierarchicalStateBase(owner) {}
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
class GryphusAttackState : public HierarchicalStateBase<GryphusStateMachine>
{
public:
    GryphusAttackState(GryphusStateMachine* owner) : HierarchicalStateBase(owner) {}
    const char* GetName() const override { return "Attack"; }
    // 開始処理
    void OnEnter() override;
    // 実行処理
    void OnExecute(float elapsedTime) override;
    // 終了処理
    void OnExit() override;
};
#pragma endregion
#pragma region 被弾
class GryphusDamageState : public HierarchicalStateBase<GryphusStateMachine>
{
public:
    GryphusDamageState(GryphusStateMachine* owner) : HierarchicalStateBase(owner) {}
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
class GryphusDeathState : public HierarchicalStateBase<GryphusStateMachine>
{
public:
    GryphusDeathState(GryphusStateMachine* owner) : HierarchicalStateBase(owner) {}
    const char* GetName() const override { return "Death"; }
    // 開始処理
    void OnEnter() override;
    // 実行処理
    void OnExecute(float elapsedTime) override;
    // 終了処理
    void OnExit() override;
};
#pragma endregion