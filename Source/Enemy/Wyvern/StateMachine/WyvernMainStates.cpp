#include "WyvernMainStates.h"

#include "../../Library/Component/Animator.h"
#include "../WyvernEnemyController.h"

#include <imgui.h>

#pragma region 待機
void WyvernIdleState::OnEnter()
{
	owner->GetAnimator()->PlayAnimation(
		u8"Idle03Shake",
		false,
		1.5f);
	owner->GetAnimator()->SetIsUseRootMotion(false);
}

void WyvernIdleState::OnExecute(float elapsedTime)
{
	// アニメーションが終了しているとき
	if (!owner->GetAnimator()->IsPlayAnimation())
	{
		// 再び待機状態
		owner->GetStateMachine().ChangeState("Idle");
	}
}

void WyvernIdleState::OnExit()
{
}
#pragma endregion

#pragma region 噛みつき攻撃
void WyvernBiteAttackState::OnEnter()
{
	// ターゲット座標から左前右のどこに攻撃するか判定
	auto& position = owner->GetWyvern()->GetActor()->GetTransform().GetPosition();
	auto& targetPosition = owner->GetWyvern()->GetTargetPosition();
	auto targetDirection = (targetPosition - position).Normalize();
	auto front = owner->GetWyvern()->GetActor()->GetTransform().GetAxisZ().Normalize();
	float crossY = front.Cross(targetDirection).y;
	if (crossY > 0.0f)
	{
		// 右に攻撃
		// アニメーション再生
		owner->GetAnimator()->PlayAnimation(
			u8"AttackBiteLeft",
			false,
			0.5f);
	}
	else
	{
		// 左に攻撃
		// アニメーション再生
		owner->GetAnimator()->PlayAnimation(
			u8"AttackBiteRight",
			false,
			0.5f);
	}
	owner->GetAnimator()->SetRootNodeIndex("CG");
	owner->GetAnimator()->SetIsUseRootMotion(true);
	owner->GetAnimator()->SetRootMotionOption(Animator::RootMotionOption::RemovePositionXY);
}
void WyvernBiteAttackState::OnExecute(float elapsedTime)
{
	// アニメーションが終了しているとき
	if (!owner->GetAnimator()->IsPlayAnimation())
	{
		// 待機状態へ遷移
		owner->GetStateMachine().ChangeState("Idle");
	}
}
void WyvernBiteAttackState::OnExit()
{
	owner->GetAnimator()->SetIsUseRootMotion(false);
}
#pragma endregion

#pragma region かぎ爪攻撃
void WyvernClawAttackState::OnEnter()
{
	// ターゲット座標から左前右のどこに攻撃するか判定
	auto& position = owner->GetWyvern()->GetActor()->GetTransform().GetPosition();
	auto& targetPosition = owner->GetWyvern()->GetTargetPosition();
	auto targetDirection = (targetPosition - position).Normalize();
	auto front = owner->GetWyvern()->GetActor()->GetTransform().GetAxisZ().Normalize();
	float crossY = front.Cross(targetDirection).y;
	if (crossY > 0.0f)
	{
		// 右に攻撃
		// アニメーション再生
		owner->GetAnimator()->PlayAnimation(
			u8"AttackWingFistRight",
			false,
			0.5f);
	}
	else
	{
		// 左に攻撃
		// アニメーション再生
		owner->GetAnimator()->PlayAnimation(
			u8"AttackWingFistLeft",
			false,
			0.5f);
	}
	owner->GetAnimator()->SetRootNodeIndex("CG");
	owner->GetAnimator()->SetIsUseRootMotion(true);
	owner->GetAnimator()->SetRootMotionOption(Animator::RootMotionOption::RemovePositionXY);
}
void WyvernClawAttackState::OnExecute(float elapsedTime)
{
	// アニメーションが終了しているとき
	if (!owner->GetAnimator()->IsPlayAnimation())
	{
		// 待機状態へ遷移
		owner->GetStateMachine().ChangeState("Idle");
	}
}
void WyvernClawAttackState::OnExit()
{
	owner->GetAnimator()->SetIsUseRootMotion(false);
}
#pragma endregion
#pragma region 尻尾攻撃
void WyvernTailAttackState::OnEnter()
{
	// ターゲット座標から左前右のどこに攻撃するか判定
	auto& position = owner->GetWyvern()->GetActor()->GetTransform().GetPosition();
	auto& targetPosition = owner->GetWyvern()->GetTargetPosition();
	auto targetDirection = (targetPosition - position).Normalize();
	auto front = owner->GetWyvern()->GetActor()->GetTransform().GetAxisZ().Normalize();
	float crossY = front.Cross(targetDirection).y;
	if (crossY > 0.0f)
	{
		// 右に攻撃
		// アニメーション再生
		owner->GetAnimator()->PlayAnimation(
			u8"AttackTailRight",
			false,
			0.5f);
	}
	else
	{
		// 左に攻撃
		// アニメーション再生
		owner->GetAnimator()->PlayAnimation(
			u8"AttackTailLeft",
			false,
			0.5f);
	}
	owner->GetAnimator()->SetRootNodeIndex("CG");
	owner->GetAnimator()->SetIsUseRootMotion(true);
	owner->GetAnimator()->SetRootMotionOption(Animator::RootMotionOption::RemovePositionXY);
}
void WyvernTailAttackState::OnExecute(float elapsedTime)
{
	// アニメーションが終了しているとき
	if (!owner->GetAnimator()->IsPlayAnimation())
	{
		// 待機状態へ遷移
		owner->GetStateMachine().ChangeState("Idle");
	}
}
void WyvernTailAttackState::OnExit()
{
	owner->GetAnimator()->SetIsUseRootMotion(false);
}
#pragma endregion

#pragma region 後退
void WyvernBackStepState::OnEnter()
{
	owner->GetAnimator()->PlayAnimation(
		u8"WalkBack",
		false,
		0.5f);
	owner->GetAnimator()->SetRootNodeIndex("CG");
	owner->GetAnimator()->SetIsUseRootMotion(true);
	owner->GetAnimator()->SetRootMotionOption(Animator::RootMotionOption::RemovePositionXY);
}
void WyvernBackStepState::OnExecute(float elapsedTime)
{
	// アニメーションが終了しているとき
	if (!owner->GetAnimator()->IsPlayAnimation())
	{
		// 待機状態へ遷移
		owner->GetStateMachine().ChangeState("Idle");
	}
}
void WyvernBackStepState::OnExit()
{
	owner->GetAnimator()->SetIsUseRootMotion(false);
}
#pragma endregion

#pragma region ブレス
void WyvernBreathAttackState::OnEnter()
{
	owner->GetAnimator()->PlayAnimation(
		u8"AttackFireBreath",
		false,
		0.5f);
	owner->GetAnimator()->SetRootNodeIndex("CG");
	owner->GetAnimator()->SetIsUseRootMotion(true);
	owner->GetAnimator()->SetRootMotionOption(Animator::RootMotionOption::RemovePositionXY);
}
void WyvernBreathAttackState::OnExecute(float elapsedTime)
{
	// TODO ブレスの生成

	// アニメーションが終了しているとき
	if (!owner->GetAnimator()->IsPlayAnimation())
	{
		// 待機状態へ遷移
		owner->GetStateMachine().ChangeState("Idle");
	}
}
void WyvernBreathAttackState::OnExit()
{
	owner->GetAnimator()->SetIsUseRootMotion(false);
}
#pragma endregion

#pragma region 近づく
void WyvernPursuitState::OnEnter()
{
	owner->GetAnimator()->PlayAnimation(
		u8"RunForward",
		true,
		0.5f);
	owner->GetAnimator()->SetRootNodeIndex("CG");
	owner->GetAnimator()->SetIsUseRootMotion(true);
	owner->GetAnimator()->SetRootMotionOption(Animator::RootMotionOption::RemovePositionXY);
}
void WyvernPursuitState::OnExecute(float elapsedTime)
{
	auto& position = owner->GetWyvern()->GetActor()->GetTransform().GetPosition();
	auto& targetPosition = owner->GetWyvern()->GetTargetPosition();
	auto targetDirection = (targetPosition - position);
	float nearAttackRange = owner->GetWyvern()->GetNearAttackRange();
	float rotationSpeed = owner->GetWyvern()->GetRotationSpeed();
	// ターゲット方向に回転
	auto charactorController = owner->GetWyvern()->GetCharactorController();
	Vector2 targetDirection2D = Vector2(targetDirection.x, targetDirection.z);
	charactorController->UpdateRotation(elapsedTime, targetDirection2D.Normalize() * rotationSpeed);
	// 現在の位置とターゲットの位置の距離から攻撃できるか判定
	if (targetDirection.Length() < nearAttackRange)
	{
		// 攻撃範囲内なら完了
		// 待機状態へ遷移
		owner->GetStateMachine().ChangeState("Idle");
		return;
	}
}
void WyvernPursuitState::OnExit()
{
	owner->GetAnimator()->SetIsUseRootMotion(false);
}
#pragma endregion

#pragma region ダメージを受ける
void WyvernDamageState::OnEnter()
{
	auto& position = owner->GetWyvern()->GetActor()->GetTransform().GetPosition();
	auto hitPosition = owner->GetWyvern()->GetHitPosition();
	auto targetDirection = (hitPosition - position).Normalize();
	auto front = owner->GetWyvern()->GetActor()->GetTransform().GetAxisZ().Normalize();
	// 被弾位置から右前、右、右後ろ、左前、左、左後ろのどこに被弾するか判定
	float crossY = front.Cross(targetDirection).y;
	float dot = front.Dot(targetDirection);
	if (crossY > 0.0f)
	{
		// 右方向に被弾
		if (dot > _frontAngleThreshold)
		{
			// 前方右に被弾
			// アニメーション再生
			owner->GetAnimator()->PlayAnimation(
				u8"DamageFrontRight",
				false,
				0.5f);
			owner->GetAnimator()->SetIsUseRootMotion(false);
		}
		else if (dot < _backAngleThreshold)
		{
			// 後方右に被弾
			// アニメーション再生
			owner->GetAnimator()->PlayAnimation(
				u8"DamageBackRight",
				false,
				0.5f);
			owner->GetAnimator()->SetIsUseRootMotion(false);
			owner->GetAnimator()->SetIsRemoveRootMovement(true);
			// アニメーションの回転量を反映させる
			_applyRotation = true;
		}
		else
		{
			// 右方向に被弾
			// アニメーション再生
			owner->GetAnimator()->PlayAnimation(
				u8"DamageRight",
				false,
				0.5f);
			owner->GetAnimator()->SetIsUseRootMotion(true);
			// アニメーションの回転量を反映させる
			_applyRotation = true;
		}
	}
	else
	{
		// 左方向に被弾
		if (dot > _frontAngleThreshold)
		{
			// 前方左に被弾
			// アニメーション再生
			owner->GetAnimator()->PlayAnimation(
				u8"DamageFrontLeft",
				false,
				0.5f);
			owner->GetAnimator()->SetIsUseRootMotion(false);
		}
		else if (dot < _backAngleThreshold)
		{
			// 後方左に被弾
			// アニメーション再生
			owner->GetAnimator()->PlayAnimation(
				u8"DamageBackLeft",
				false,
				0.5f);
			owner->GetAnimator()->SetIsUseRootMotion(false);
			owner->GetAnimator()->SetIsRemoveRootMovement(true);
			// アニメーションの回転量を反映させる
			_applyRotation = true;
		}
		else
		{
			// 左方向に被弾
			// アニメーション再生
			owner->GetAnimator()->PlayAnimation(
				u8"DamageLeft",
				false,
				0.5f);
			owner->GetAnimator()->SetIsUseRootMotion(true);
			// アニメーションの回転量を反映させる
			_applyRotation = true;
		}
	}
	owner->GetAnimator()->SetRootNodeIndex("CG");
	owner->GetAnimator()->SetRootMotionOption(Animator::RootMotionOption::RemovePositionXY);
}
void WyvernDamageState::OnExecute(float elapsedTime)
{
	// アニメーションが終了しているときかキャンセルイベントが発生しているとき
	if (!owner->GetAnimator()->IsPlayAnimation() || owner->CallCancelEvent())
	{
		// 待機状態へ遷移
		owner->GetStateMachine().ChangeState("Idle");
	}
}
void WyvernDamageState::OnExit()
{
	// 現在のアニメーションの回転量を取り除き、アクターの回転に反映する
	if (_applyRotation)
	{
		int rootNodeIndex = owner->GetWyvern()->GetActor()->GetModel().lock()->GetNodeIndex("CG");
		auto& poseNodes = owner->GetWyvern()->GetActor()->GetModel().lock()->GetPoseNodes();
		int animationIndex = owner->GetAnimator()->GetAnimationIndex();
		ModelResource::Node startRootNode{};
		owner->GetAnimator()->ComputeAnimation(animationIndex, rootNodeIndex, 0.0f, startRootNode);
		// 回転量の差分を求める
		Quaternion q = Quaternion::Multiply(
			Quaternion::Inverse(poseNodes[rootNodeIndex].rotation),
			startRootNode.rotation);
		// ルートの回転量を取り除く
		poseNodes[rootNodeIndex].rotation = startRootNode.rotation;
		owner->GetWyvern()->GetActor()->GetModel().lock()->SetPoseNodes(poseNodes);

		// 回転量をアクターに反映する
		auto& transform = owner->GetWyvern()->GetActor()->GetTransform();
		Vector3 angle{};
		// ルートの行列の都合上z値をyに設定
		angle.y = q.ToRollPitchYaw().z;
		transform.AddRotation(angle);

		// フラグを下ろす
		_applyRotation = false;
		owner->GetAnimator()->SetIsRemoveRootMovement(false);
	}

	owner->GetAnimator()->SetIsUseRootMotion(false);
}
#pragma endregion
#pragma region 死亡
void WyvernDeathState::OnEnter()
{
	owner->GetAnimator()->PlayAnimation(
		u8"Death01",
		false,
		0.5f);
	owner->GetAnimator()->SetRootNodeIndex("CG");
	owner->GetAnimator()->SetIsUseRootMotion(true);
	owner->GetAnimator()->SetRootMotionOption(Animator::RootMotionOption::RemovePositionXY);
}
void WyvernDeathState::OnExecute(float elapsedTime)
{
}
void WyvernDeathState::OnExit()
{
	owner->GetAnimator()->SetIsUseRootMotion(false);
}
#pragma endregion
