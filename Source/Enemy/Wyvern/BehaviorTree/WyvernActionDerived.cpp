#include "WyvernActionDerived.h"

#include "WyvernBehaviorTree.h"
#include "../WyvernEnemyController.h"

// 開始処理
void WyvernNormalAction::OnEnter()
{
	static const std::string AttackAnimationNames[] =
	{
		u8"AttackBiteFront",
		u8"AttackBiteLeft",
		u8"AttackBiteRight",
		u8"AttackFireBall",
		u8"AttackFireBreath",
		u8"AttackFireSwipe",
		u8"AttackSpecial1",
		u8"AttackTailLeft",
		u8"AttackTailRight",
		u8"AttackWingFistLeft",
		u8"AttackWingFistRight",
		u8"AttackWingSwipeLeft",
		u8"AttackWingSwipeRight",
	};
	// アニメーション再生
	_owner->GetAnimator()->PlayAnimation(
		AttackAnimationNames[std::rand() % _countof(AttackAnimationNames)],
		false,
		0.5f);
	_owner->GetAnimator()->SetRootNodeIndex("CG");
	_owner->GetAnimator()->SetIsUseRootMotion(true);
	_owner->GetAnimator()->SetRootMotionOption(Animator::RootMotionOption::RemovePositionXY);
}

BehaviorActionState WyvernNormalAction::Run(float elapsedTime)
{
	// アニメーションが終了しているとき
	if (!_owner->GetAnimator()->IsPlayAnimation())
	{
		// 攻撃成功を返す
		return BehaviorActionState::Complete;
	}
	return BehaviorActionState::Run;
}

#pragma region 噛みつき攻撃
void WyvernBiteAction::OnEnter()
{
	// ターゲット座標から左前右のどこに攻撃するか判定
	auto& position = _owner->GetWyvern()->GetActor()->GetTransform().GetPosition();
	auto& targetPosition = _owner->GetWyvern()->GetTargetPosition();
	auto targetDirection = (targetPosition - position).Normalize();
	auto front = _owner->GetWyvern()->GetActor()->GetTransform().GetAxisZ().Normalize();
	float crossY = front.Cross(targetDirection).y;
	if (crossY > 0.0f)
	{
		// 右に攻撃
		// アニメーション再生
		_owner->GetAnimator()->PlayAnimation(
			u8"AttackBiteLeft",
			false,
			0.5f);
	}
	else
	{
		// 左に攻撃
		// アニメーション再生
		_owner->GetAnimator()->PlayAnimation(
			u8"AttackBiteRight",
			false,
			0.5f);
	}
	_owner->GetAnimator()->SetRootNodeIndex("CG");
	_owner->GetAnimator()->SetIsUseRootMotion(true);
	_owner->GetAnimator()->SetRootMotionOption(Animator::RootMotionOption::RemovePositionXY);
}

BehaviorActionState WyvernBiteAction::Run(float elapsedTime)
{
	// アニメーションが終了しているとき
	if (!_owner->GetAnimator()->IsPlayAnimation())
	{
		// 攻撃成功を返す
		return BehaviorActionState::Complete;
	}
	return BehaviorActionState::Run;
}
#pragma endregion

#pragma region かぎ爪攻撃
void WyvernClawAction::OnEnter()
{
	// ターゲット座標から左前右のどこに攻撃するか判定
	auto& position = _owner->GetWyvern()->GetActor()->GetTransform().GetPosition();
	auto& targetPosition = _owner->GetWyvern()->GetTargetPosition();
	auto targetDirection = (targetPosition - position).Normalize();
	auto front = _owner->GetWyvern()->GetActor()->GetTransform().GetAxisZ().Normalize();
	float crossY = front.Cross(targetDirection).y;
	if (crossY > 0.0f)
	{
		// 右に攻撃
		// アニメーション再生
		_owner->GetAnimator()->PlayAnimation(
			u8"AttackWingFistRight",
			false,
			0.5f);
	}
	else
	{
		// 左に攻撃
		// アニメーション再生
		_owner->GetAnimator()->PlayAnimation(
			u8"AttackWingFistLeft",
			false,
			0.5f);
	}
	_owner->GetAnimator()->SetRootNodeIndex("CG");
	_owner->GetAnimator()->SetIsUseRootMotion(true);
	_owner->GetAnimator()->SetRootMotionOption(Animator::RootMotionOption::RemovePositionXY);
}

BehaviorActionState WyvernClawAction::Run(float elapsedTime)
{
	// アニメーションが終了しているとき
	if (!_owner->GetAnimator()->IsPlayAnimation())
	{
		// 攻撃成功を返す
		return BehaviorActionState::Complete;
	}
	return BehaviorActionState::Run;
}
#pragma endregion

#pragma region 尻尾攻撃
void WyvernTailAction::OnEnter()
{
	// ターゲット座標から左前右のどこに攻撃するか判定
	auto& position = _owner->GetWyvern()->GetActor()->GetTransform().GetPosition();
	auto& targetPosition = _owner->GetWyvern()->GetTargetPosition();
	auto targetDirection = (targetPosition - position).Normalize();
	auto front = _owner->GetWyvern()->GetActor()->GetTransform().GetAxisZ().Normalize();
	float crossY = front.Cross(targetDirection).y;
	if (crossY > 0.0f)
	{
		// 右に攻撃
		// アニメーション再生
		_owner->GetAnimator()->PlayAnimation(
			u8"AttackTailRight",
			false,
			0.5f);
	}
	else
	{
		// 左に攻撃
		// アニメーション再生
		_owner->GetAnimator()->PlayAnimation(
			u8"AttackTailLeft",
			false,
			0.5f);
	}
	_owner->GetAnimator()->SetRootNodeIndex("CG");
	_owner->GetAnimator()->SetIsUseRootMotion(true);
	_owner->GetAnimator()->SetRootMotionOption(Animator::RootMotionOption::RemovePositionXY);
}

BehaviorActionState WyvernTailAction::Run(float elapsedTime)
{
	// アニメーションが終了しているとき
	if (!_owner->GetAnimator()->IsPlayAnimation())
	{
		// 攻撃成功を返す
		return BehaviorActionState::Complete;
	}
	return BehaviorActionState::Run;
}
#pragma endregion

#pragma region 後退
void WyvernBackStepAction::OnEnter()
{
	_owner->GetAnimator()->PlayAnimation(
		u8"WalkBack",
		false,
		0.5f);
	_owner->GetAnimator()->SetRootNodeIndex("CG");
	_owner->GetAnimator()->SetIsUseRootMotion(true);
	_owner->GetAnimator()->SetRootMotionOption(Animator::RootMotionOption::RemovePositionXY);
}

BehaviorActionState WyvernBackStepAction::Run(float elapsedTime)
{
	// アニメーションが終了しているとき
	if (!_owner->GetAnimator()->IsPlayAnimation())
	{
		// 攻撃成功を返す
		return BehaviorActionState::Complete;
	}
	return BehaviorActionState::Run;
}
#pragma endregion

#pragma region ブレス
void WyvernBreathAction::OnEnter()
{
	_owner->GetAnimator()->PlayAnimation(
		u8"AttackFireBreath",
		false,
		0.5f);
	_owner->GetAnimator()->SetRootNodeIndex("CG");
	_owner->GetAnimator()->SetIsUseRootMotion(true);
	_owner->GetAnimator()->SetRootMotionOption(Animator::RootMotionOption::RemovePositionXY);
}

BehaviorActionState WyvernBreathAction::Run(float elapsedTime)
{
	// TODO ブレスの生成

	// アニメーションが終了しているとき
	if (!_owner->GetAnimator()->IsPlayAnimation())
	{
		// 攻撃成功を返す
		return BehaviorActionState::Complete;
	}
	return BehaviorActionState::Run;
}
#pragma endregion

#pragma region とびかかり
void WyvernJumpOnAction::OnEnter()
{
	// TODO : とびかかりアニメーション

	_owner->GetAnimator()->PlayAnimation(
		u8"AttackFireBreath",
		false,
		0.5f);
	_owner->GetAnimator()->SetRootNodeIndex("CG");
	_owner->GetAnimator()->SetIsUseRootMotion(true);
	_owner->GetAnimator()->SetRootMotionOption(Animator::RootMotionOption::RemovePositionXY);
}

BehaviorActionState WyvernJumpOnAction::Run(float elapsedTime)
{
	// アニメーションが終了しているとき
	if (!_owner->GetAnimator()->IsPlayAnimation())
	{
		// 攻撃成功を返す
		return BehaviorActionState::Complete;
	}
	return BehaviorActionState::Run;
}
#pragma endregion

#pragma region 近づく
void WyvernPursuitAction::OnEnter()
{
	_owner->GetAnimator()->PlayAnimation(
		u8"RunForward",
		true,
		0.5f);
	_owner->GetAnimator()->SetRootNodeIndex("CG");
	_owner->GetAnimator()->SetIsUseRootMotion(true);
	_owner->GetAnimator()->SetRootMotionOption(Animator::RootMotionOption::RemovePositionXY);
}

BehaviorActionState WyvernPursuitAction::Run(float elapsedTime)
{
	auto& position = _owner->GetWyvern()->GetActor()->GetTransform().GetPosition();
	auto& targetPosition = _owner->GetWyvern()->GetTargetPosition();
	auto targetDirection = (targetPosition - position);
	float nearAttackRange = _owner->GetWyvern()->GetNearAttackRange();
	float rotationSpeed = _owner->GetWyvern()->GetRotationSpeed();

	// ターゲット方向に回転
	auto charactorController = _owner->GetWyvern()->GetCharactorController();
	Vector2 targetDirection2D = Vector2(targetDirection.x, targetDirection.z);
	charactorController->UpdateRotation(elapsedTime, targetDirection2D.Normalize() * rotationSpeed);

	// 現在の位置とターゲットの位置の距離から攻撃できるか判定
	if (targetDirection.Length() < nearAttackRange)
	{
		// 攻撃範囲内なら完了
		return BehaviorActionState::Complete;
	}
	return BehaviorActionState::Run;
}
#pragma endregion

#pragma region 待機
void WyvernIdleAction::OnEnter()
{
	_owner->GetAnimator()->PlayAnimation(
		u8"Idle03Shake",
		false,
		0.5f);
	_owner->GetAnimator()->SetIsUseRootMotion(false);
}

BehaviorActionState WyvernIdleAction::Run(float elapsedTime)
{
	// アニメーションが終了しているとき
	if (!_owner->GetAnimator()->IsPlayAnimation())
	{
		// 成功を返す
		return BehaviorActionState::Complete;
	}
	return BehaviorActionState::Run;
}
#pragma endregion
