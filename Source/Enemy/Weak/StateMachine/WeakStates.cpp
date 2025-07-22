#include "WeakStates.h"

#include "../../Library/Component/Animator.h"
#include "../../Library/Scene/Scene.h"
#include "../../EnemyController.h"
#include "../WeakController.h"

#include "../../Library/DebugSupporter/DebugSupporter.h"

#include <imgui.h>

#pragma region 待機
void WeakIdleState::OnEnter()
{
	owner->GetAnimator()->PlayAnimation(
		u8"Idle",
		false,
		0.0f);
	owner->GetAnimator()->SetIsUseRootMotion(false);
}

void WeakIdleState::OnExecute(float elapsedTime)
{
	// アニメーションが終了しているとき
	if (!owner->GetAnimator()->IsPlayAnimation())
	{
		// 再び待機状態
		owner->GetBase().ChangeState("Idle");
	}
}

void WeakIdleState::OnExit()
{
}
#pragma endregion
#pragma region 休憩
void WeakRestState::OnEnter()
{
	owner->GetAnimator()->PlayAnimation(
		u8"Idle",
		false,
		0.0f);
	owner->GetAnimator()->SetIsUseRootMotion(false);
}

void WeakRestState::OnExecute(float elapsedTime)
{
	// アニメーションが終了しているとき
	if (!owner->GetAnimator()->IsPlayAnimation())
	{
		// 待機状態へ遷移
		owner->GetBase().ChangeState("Idle");
	}
}

void WeakRestState::OnExit()
{
}
#pragma endregion
#pragma region 威嚇
void WeakThreatState::OnEnter()
{
	owner->GetAnimator()->PlayAnimation(
		u8"Idle",
		true,
		0.0f);
	owner->GetAnimator()->SetIsUseRootMotion(false);
}

void WeakThreatState::OnExecute(float elapsedTime)
{
	// クールタイムが終了したら遷移
	if (!owner->GetEnemy()->IsAttackCooldown())
	{
		// ターゲットの範囲内にいる場合、待機状態へ遷移
		owner->GetBase().ChangeState("Idle");
		return;
	}
}

void WeakThreatState::OnExit()
{
}
#pragma endregion
#pragma region 移動
void WeakMoveState::OnEnter()
{
	owner->GetAnimator()->PlayAnimation(
		u8"Jog",
		true,
		0.0f);
	owner->GetAnimator()->SetIsUseRootMotion(false);
}
void WeakMoveState::OnExecute(float elapsedTime)
{
	auto& targetPosition = owner->GetEnemy()->GetTargetPosition();
	Vector3 vec = targetPosition - owner->GetEnemy()->GetActor()->GetTransform().GetPosition();
	float length = vec.Length();

	bool isTargetInRange = owner->GetEnemy()->IsInTargetRange(targetPosition, owner->GetEnemy()->GetNearAttackRange());
	bool isLookAtTarget = owner->GetEnemy()->GetAngleToTarget(targetPosition) < owner->GetEnemy()->GetLookAtRadian();

	// ターゲットが攻撃範囲外なら移動処理
	if (!isTargetInRange)
	{
		float jogSpeed = owner->GetWeak()->GetJogSpeed();
		// ターゲット方向に移動
		owner->GetEnemy()->MoveToTarget(targetPosition, jogSpeed);
	}
	else
	{
		// ターゲットが攻撃範囲内なら移動しない
		owner->GetEnemy()->GetCharactorController()->SetVelocityXZ(0.0f, 0.0f);
	}

	// ターゲット方向に向いていないなら回転処理
	if (!isLookAtTarget)
	{
		float rotationSpeed = owner->GetEnemy()->GetRotationSpeed();
		// ターゲット方向に回転
		owner->GetEnemy()->LookAtTarget(targetPosition, elapsedTime, rotationSpeed);
	}

	// 移動と回転が完了しているかなら遷移
	if (isTargetInRange && isLookAtTarget)
	{
		// ターゲットの範囲内にいる場合、待機状態へ遷移
		owner->GetBase().ChangeState("Idle");
		return;
	}
}
void WeakMoveState::OnExit()
{
	// 速度をリセット
	owner->GetEnemy()->GetCharactorController()->SetVelocityXZ(0.0f, 0.0f);

	owner->GetAnimator()->SetIsUseRootMotion(false);
}
#pragma endregion
#pragma region ダッシュ
void WeakDashState::OnEnter()
{
	owner->GetAnimator()->PlayAnimation(
		u8"Run",
		true,
		0.0f);
	owner->GetAnimator()->SetIsUseRootMotion(false);
}
void WeakDashState::OnExecute(float elapsedTime)
{
	auto& targetPosition = owner->GetEnemy()->GetTargetPosition();
	Vector3 vec = targetPosition - owner->GetEnemy()->GetActor()->GetTransform().GetPosition();
	float length = vec.Length();

	bool isTargetInRange = owner->GetEnemy()->IsInTargetRange(targetPosition, owner->GetEnemy()->GetNearAttackRange());
	bool isLookAtTarget = owner->GetEnemy()->GetAngleToTarget(targetPosition) < owner->GetEnemy()->GetLookAtRadian();

	// ターゲットが攻撃範囲外なら移動処理
	if (!isTargetInRange)
	{
		float dashSpeed = owner->GetWeak()->GetDashSpeed();
		// ターゲット方向に移動
		owner->GetEnemy()->MoveToTarget(targetPosition, dashSpeed);
	}
	else
	{
		// ターゲットが攻撃範囲内なら移動しない
		owner->GetEnemy()->GetCharactorController()->SetVelocityXZ(0.0f, 0.0f);
	}

	// ターゲット方向に向いていないなら回転処理
	if (!isLookAtTarget)
	{
		float rotationSpeed = owner->GetEnemy()->GetRotationSpeed();
		// ターゲット方向に回転
		owner->GetEnemy()->LookAtTarget(targetPosition, elapsedTime, rotationSpeed);
	}

	// 移動と回転が完了しているかなら遷移
	if (isTargetInRange && isLookAtTarget)
	{
		// ターゲットの範囲内にいる場合、待機状態へ遷移
		owner->GetBase().ChangeState("Idle");
		return;
	}
}
void WeakDashState::OnExit()
{
	// 速度をリセット
	owner->GetEnemy()->GetCharactorController()->SetVelocityXZ(0.0f, 0.0f);

	owner->GetAnimator()->SetIsUseRootMotion(false);
}
#pragma endregion
#pragma region 攻撃
void WeakAttackState::OnEnter()
{
	// 左右をランダムに攻撃
	if (rand() % 2 == 0)
	{
		owner->GetAnimator()->PlayAnimation(
			u8"AttackL",
			false,
			0.0f);
	}
	else
	{
		owner->GetAnimator()->PlayAnimation(
			u8"AttackR",
			false,
			0.0f);
	}

	owner->GetAnimator()->SetIsUseRootMotion(false);
}
void WeakAttackState::OnExecute(float elapsedTime)
{
	// アニメーションが終了しているとき
	if (!owner->GetAnimator()->IsPlayAnimation())
	{
		// 待機状態へ遷移
		owner->GetBase().ChangeState("Idle");
	}
}
void WeakAttackState::OnExit()
{
	// 攻撃後の待機時間を設定
	owner->GetEnemy()->SetAttackCooldown(_waitTime);
	owner->GetAnimator()->SetIsUseRootMotion(false);
}
#pragma endregion
#pragma region ジャンプ攻撃
void WeakJumpAttackState::OnEnter()
{
	owner->GetAnimator()->PlayAnimation(
		u8"AttackJump",
		false,
		0.5f);
	owner->GetAnimator()->SetRootNodeIndex("Raptor_Hub001");
	owner->GetAnimator()->SetRootMotionOption(Animator::RootMotionOption::RemovePositionX);
	owner->GetAnimator()->SetIsUseRootMotion(true);
}
void WeakJumpAttackState::OnExecute(float elapsedTime)
{
	// アニメーションが終了しているとき
	if (!owner->GetAnimator()->IsPlayAnimation())
	{
		// 待機状態へ遷移
		owner->GetBase().ChangeState("Idle");
	}
}
void WeakJumpAttackState::OnExit()
{
	// 攻撃後の待機時間を設定
	owner->GetEnemy()->SetAttackCooldown(_waitTime);
	owner->GetAnimator()->SetIsUseRootMotion(false);
}
#pragma endregion
#pragma region 被弾
void WeakDamageState::OnEnter()
{
	// HitBack HitFront HitSide 
	owner->GetAnimator()->PlayAnimation(
		u8"HitBack",
		false,
		0.0f);
	owner->GetAnimator()->SetIsUseRootMotion(false);
}
void WeakDamageState::OnExecute(float elapsedTime)
{
	// アニメーションが終了しているとき
	if (!owner->GetAnimator()->IsPlayAnimation())
	{
		// 待機状態へ遷移
		owner->GetBase().ChangeState("Idle");
	}
}
void WeakDamageState::OnExit()
{
	owner->GetAnimator()->SetIsUseRootMotion(false);
}
#pragma endregion
#pragma region 死亡
void WeakDeathState::OnEnter()
{
	// DeathL DeathR DeathFront
	owner->GetAnimator()->PlayAnimation(
		u8"DeathFront",
		false,
		0.0f);
	owner->GetAnimator()->SetIsUseRootMotion(false);
}
void WeakDeathState::OnExecute(float elapsedTime)
{
}
void WeakDeathState::OnExit()
{
	owner->GetAnimator()->SetIsUseRootMotion(false);
}
#pragma endregion
