#include "GryphusStates.h"

#include "../../Library/Component/Animator.h"
#include "../../Library/Scene/Scene.h"
#include "../../EnemyController.h"
#include "../GryphusController.h"

#include "../../Library/DebugSupporter/DebugSupporter.h"

#include <imgui.h>

#pragma region 待機
void GryphusIdleState::OnEnter()
{
    owner->GetAnimator()->PlayAnimation(
        u8"Idle",
        true,
        0.0f);
    owner->GetAnimator()->SetIsUseRootMotion(false);
}
void GryphusIdleState::OnExecute(float elapsedTime)
{
    // アニメーションが終了しているとき
    if (!owner->GetAnimator()->IsPlayAnimation())
    {
        // 再び待機状態
        owner->GetBase().ChangeState("Idle");
    }
}
void GryphusIdleState::OnExit()
{
    owner->GetAnimator()->SetIsUseRootMotion(false);
}
#pragma endregion
#pragma region 休憩
void GryphusRestState::OnEnter()
{
    owner->GetAnimator()->PlayAnimation(
        u8"Idle",
        true,
        0.0f);
    owner->GetAnimator()->SetIsUseRootMotion(false);
}
void GryphusRestState::OnExecute(float elapsedTime)
{
    // アニメーションが終了しているとき
    if (!owner->GetAnimator()->IsPlayAnimation())
    {
        // 待機状態へ遷移
        owner->GetBase().ChangeState("Idle");
    }
}
void GryphusRestState::OnExit()
{
    owner->GetAnimator()->SetIsUseRootMotion(false);
}
#pragma endregion
#pragma region 威嚇
void GryphusThreatState::OnEnter()
{
    owner->GetAnimator()->PlayAnimation(
        u8"Idle",
        true,
        0.0f);
    owner->GetAnimator()->SetIsUseRootMotion(false);
}
void GryphusThreatState::OnExecute(float elapsedTime)
{
    // アニメーションが終了しているとき
    if (!owner->GetAnimator()->IsPlayAnimation())
    {
        // 待機状態へ遷移
        owner->GetBase().ChangeState("Idle");
    }
}
void GryphusThreatState::OnExit()
{
    owner->GetAnimator()->SetIsUseRootMotion(false);
}
#pragma endregion
#pragma region 移動
void GryphusMoveState::OnEnter()
{
    owner->GetAnimator()->PlayAnimation(
        u8"Run",
        true,
        0.0f);
    owner->GetAnimator()->SetIsUseRootMotion(false);
}
void GryphusMoveState::OnExecute(float elapsedTime)
{
    // アニメーションが終了しているとき
    if (!owner->GetAnimator()->IsPlayAnimation())
    {
        // 待機状態へ遷移
        owner->GetBase().ChangeState("Idle");
    }
}
void GryphusMoveState::OnExit()
{
    owner->GetAnimator()->SetIsUseRootMotion(false);
}
#pragma endregion
#pragma region ダッシュ
void GryphusDashState::OnEnter()
{
    owner->GetAnimator()->PlayAnimation(
        u8"Run",
        true,
        0.0f);
    owner->GetAnimator()->SetIsUseRootMotion(false);
}
void GryphusDashState::OnExecute(float elapsedTime)
{
    // アニメーションが終了しているとき
    if (!owner->GetAnimator()->IsPlayAnimation())
    {
        // 待機状態へ遷移
        owner->GetBase().ChangeState("Idle");
    }
}
void GryphusDashState::OnExit()
{
    owner->GetAnimator()->SetIsUseRootMotion(false);
}
#pragma endregion
#pragma region 攻撃
void GryphusAttackState::OnEnter()
{
    owner->GetAnimator()->PlayAnimation(
        u8"Attack",
        false,
        0.0f);
    owner->GetAnimator()->SetIsUseRootMotion(true);
}
void GryphusAttackState::OnExecute(float elapsedTime)
{
    // アニメーションが終了しているとき
    if (!owner->GetAnimator()->IsPlayAnimation())
    {
        // 待機状態へ遷移
        owner->GetBase().ChangeState("Idle");
    }
}
void GryphusAttackState::OnExit()
{
    // 攻撃後の待機時間を設定
    //owner->GetEnemy()->SetAttackCooldown(_waitTime);
    owner->GetAnimator()->SetIsUseRootMotion(false);
}
#pragma endregion
#pragma region 被弾
void GryphusDamageState::OnEnter()
{
    // HitBack HitFront HitSide 
    owner->GetAnimator()->PlayAnimation(
        u8"HitBack",
        false,
        0.0f);
    owner->GetAnimator()->SetIsUseRootMotion(false);
}
void GryphusDamageState::OnExecute(float elapsedTime)
{
    // アニメーションが終了しているとき
    if (!owner->GetAnimator()->IsPlayAnimation())
    {
        // 待機状態へ遷移
        owner->GetBase().ChangeState("Idle");
    }
}
void GryphusDamageState::OnExit()
{
    owner->GetAnimator()->SetIsUseRootMotion(false);
}
#pragma endregion
#pragma region 死亡
void GryphusDeathState::OnEnter()
{
    // DeathL DeathR DeathFront
    owner->GetAnimator()->PlayAnimation(
        u8"DeathFront",
        false,
        0.0f);
    owner->GetAnimator()->SetIsUseRootMotion(false);
}
void GryphusDeathState::OnExecute(float elapsedTime)
{
    // アニメーションが終了しているとき
    if (!owner->GetAnimator()->IsPlayAnimation())
    {
        // 死亡状態へ遷移
        owner->GetBase().ChangeState("Death");
    }
}
void GryphusDeathState::OnExit()
{
    owner->GetAnimator()->SetIsUseRootMotion(false);
}
#pragma endregion
