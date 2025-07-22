#pragma once

#include "../../Library/Component/StateController.h"
#include "../../Library/Math/Vector.h"
#include "../../Source/Common/Damageable.h"

// 前方宣言
class EnemyController;
class GryphusController;
class Animator;

class GryphusStateMachine : public StateMachine
{
public:
    GryphusStateMachine(
        EnemyController* enemy,
        GryphusController* gryphus,
        Animator* animator,
        Damageable* damageable);
    ~GryphusStateMachine() {}
    // 開始処理
    void Start() override;
    // 実行処理
    void Execute(float elapsedTime) override;
    // Gui描画
    void DrawGui() override;
#pragma region アクセサ
    // ステート変更
    void ChangeState(const char* mainStateName, const char* subStateName) override;
    // ステート名取得
    const char* GetStateName() override;
    // サブステート名取得
    const char* GetSubStateName() override;
    StateMachineBase<GryphusStateMachine>& GetBase() { return _stateMachine; }
    EnemyController* GetEnemy() { return _enemy; }
    GryphusController* GetGryphus() { return _gryphus; }
    Animator* GetAnimator() { return _animator; }
    Damageable* GetDamageable() { return _damageable; }
#pragma endregion
private:
    StateMachineBase<GryphusStateMachine> _stateMachine;
    EnemyController* _enemy = nullptr;
    GryphusController* _gryphus = nullptr;
    Animator* _animator = nullptr;
    Damageable* _damageable = nullptr;
};