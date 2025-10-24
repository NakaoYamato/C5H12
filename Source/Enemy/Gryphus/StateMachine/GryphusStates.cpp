#include "GryphusStates.h"

#include "../../Library/Component/Animator.h"
#include "../../Library/Scene/Scene.h"
#include "../../EnemyController.h"
#include "../GryphusController.h"

#include <imgui.h>

#pragma region 待機
void GryphusIdleState::OnEnter()
{
    _owner->GetAnimator()->PlayAnimation(
        u8"Idle",
        true,
        1.0f);
    _owner->GetAnimator()->SetIsUseRootMotion(false);
}
void GryphusIdleState::OnExecute(float elapsedTime)
{
    // アニメーションが終了しているとき
    if (!_owner->GetAnimator()->IsPlayAnimation())
    {
        // 再び待機状態
        _owner->GetBase().ChangeState("Idle");
    }
}
void GryphusIdleState::OnExit()
{
    _owner->GetAnimator()->SetIsUseRootMotion(false);
}
#pragma endregion
#pragma region 休憩
void GryphusRestState::OnEnter()
{
    _owner->GetAnimator()->PlayAnimation(
        u8"Idle",
        false,
        1.0f);
    _owner->GetAnimator()->SetIsUseRootMotion(false);
}
void GryphusRestState::OnExecute(float elapsedTime)
{
    // アニメーションが終了しているとき
    if (!_owner->GetAnimator()->IsPlayAnimation())
    {
        // 待機状態へ遷移
        _owner->GetBase().ChangeState("Idle");
    }
}
void GryphusRestState::OnExit()
{
    _owner->GetAnimator()->SetIsUseRootMotion(false);
}
#pragma endregion
#pragma region 威嚇
void GryphusThreatState::OnEnter()
{
    _owner->GetAnimator()->PlayAnimation(
        u8"Combat",
        false,
        1.0f);
    _owner->GetAnimator()->SetIsUseRootMotion(false);
}
void GryphusThreatState::OnExecute(float elapsedTime)
{
    // アニメーションが終了しているとき
    if (!_owner->GetAnimator()->IsPlayAnimation())
    {
        // 待機状態へ遷移
        _owner->GetBase().ChangeState("Idle");
    }
}
void GryphusThreatState::OnExit()
{
    _owner->GetAnimator()->SetIsUseRootMotion(false);
}
#pragma endregion
#pragma region 移動
void GryphusMoveState::OnEnter()
{
    // Walk02
    _owner->GetAnimator()->PlayAnimation(
        u8"Walk",
        true,
        1.0f);
    _owner->GetAnimator()->SetIsUseRootMotion(false);
}
void GryphusMoveState::OnExecute(float elapsedTime)
{
    // アニメーションが終了しているとき
    if (!_owner->GetAnimator()->IsPlayAnimation())
    {
        // 待機状態へ遷移
        _owner->GetBase().ChangeState("Idle");
    }
}
void GryphusMoveState::OnExit()
{
    _owner->GetAnimator()->SetIsUseRootMotion(false);
}
#pragma endregion
#pragma region ダッシュ
void GryphusDashState::OnEnter()
{
    // RunStart
    _owner->GetAnimator()->PlayAnimation(
        u8"Run",
        true,
        1.0f);
    _owner->GetAnimator()->SetIsUseRootMotion(false);
}
void GryphusDashState::OnExecute(float elapsedTime)
{
    // アニメーションが終了しているとき
    if (!_owner->GetAnimator()->IsPlayAnimation())
    {
        // 待機状態へ遷移
        _owner->GetBase().ChangeState("Idle");
    }
}
void GryphusDashState::OnExit()
{
    _owner->GetAnimator()->SetIsUseRootMotion(false);
}
#pragma endregion
#pragma region 攻撃
void GryphusAttackState::OnEnter()
{
    _owner->GetAnimator()->PlayAnimation(
        u8"AttackWing",
        false,
        1.0f);
    _owner->GetAnimator()->SetIsUseRootMotion(true);
}
void GryphusAttackState::OnExecute(float elapsedTime)
{
    // アニメーションが終了しているとき
    if (!_owner->GetAnimator()->IsPlayAnimation())
    {
        // 待機状態へ遷移
        _owner->GetBase().ChangeState("Idle");
    }
}
void GryphusAttackState::OnExit()
{
    // 攻撃後の待機時間を設定
    //_owner->GetEnemy()->SetAttackCooldown(_waitTime);
    _owner->GetAnimator()->SetIsUseRootMotion(false);
}
#pragma endregion
#pragma region 被弾
void GryphusDamageState::OnEnter()
{
    //  SlapFront SlapFrontStrong
    _owner->GetAnimator()->PlayAnimation(
        u8"SlapFront",
        false,
        1.0f);
    _owner->GetAnimator()->SetIsUseRootMotion(false);
}
void GryphusDamageState::OnExecute(float elapsedTime)
{
    // アニメーションが終了しているとき
    if (!_owner->GetAnimator()->IsPlayAnimation())
    {
        // 待機状態へ遷移
        _owner->GetBase().ChangeState("Idle");
    }
}
void GryphusDamageState::OnExit()
{
    _owner->GetAnimator()->SetIsUseRootMotion(false);
}
#pragma endregion
#pragma region 死亡
void GryphusDeathState::OnEnter()
{
    // DeathAir
    _owner->GetAnimator()->PlayAnimation(
        u8"Death",
        false,
        1.0f);
    _owner->GetAnimator()->SetIsUseRootMotion(false);
}
void GryphusDeathState::OnExecute(float elapsedTime)
{
}
void GryphusDeathState::OnExit()
{
    _owner->GetAnimator()->SetIsUseRootMotion(false);
}
#pragma endregion
