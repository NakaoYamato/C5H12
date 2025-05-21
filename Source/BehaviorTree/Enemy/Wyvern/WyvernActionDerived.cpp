#include "WyvernActionDerived.h"

#include "../../Source/BehaviorTree/Enemy/Wyvern/WyvernBehaviorTree.h"
#include "../../Source/Component/Enemy/Wyvern/WyvernEnemyController.h"

BehaviorActionState WyvernNormalAction::Run(float elapsedTime)
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

	switch (_step)
	{
	case 0:
		// アニメーション再生
		_owner->GetAnimator()->PlayAnimation(
			AttackAnimationNames[std::rand() % _countof(AttackAnimationNames)],
			false,
			0.5f);
		_step++;
		break;
	case 1:

		// アニメーションが終了しているとき
		if (!_owner->GetAnimator()->IsPlayAnimation())
		{
			_step = 0;
			// 攻撃成功を返す
			return BehaviorActionState::Complete;
		}
		break;
	}
	return BehaviorActionState::Run;
}

BehaviorActionState WyvernIdleAction::Run(float elapsedTime)
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

	switch (_step)
	{
	case 0:
		// アニメーション再生
		_owner->GetAnimator()->PlayAnimation(
			AttackAnimationNames[std::rand() % _countof(AttackAnimationNames)],
			false,
			0.5f);
		_step++;
		break;
	case 1:

		// アニメーションが終了しているとき
		if (!_owner->GetAnimator()->IsPlayAnimation())
		{
			_step = 0;
			// 攻撃成功を返す
			return BehaviorActionState::Complete;
		}
		break;
	}
	return BehaviorActionState::Run;
}
