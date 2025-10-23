#include "WyvernHoverState.h"

#include "../../Library/Component/Animator.h"
#include "../../Library/Scene/Scene.h"
#include "../WyvernController.h"
#include "../../Library/DebugSupporter/DebugSupporter.h"

#include <imgui.h>

#pragma region 滞空
void WyvernHoverIdleState::OnEnter()
{
	_owner->GetAnimator()->PlayAnimation(
		u8"FlyStand",
		true,
		1.5f);
}

void WyvernHoverIdleState::OnExecute(float elapsedTime)
{
}

void WyvernHoverIdleState::OnExit()
{
}
#pragma endregion

#pragma region 空中移動
void WyvernHoverToTargetState::OnEnter()
{
	_owner->GetAnimator()->PlayAnimation(
		u8"FlyForward",
		false,
		1.5f);
	_owner->GetAnimator()->SetIsUseRootMotion(false);
}

void WyvernHoverToTargetState::OnExecute(float elapsedTime)
{
	auto& targetPosition = _owner->GetCombatStatus()->GetTargetPosition();
	float rotationSpeed = _owner->GetEnemy()->GetRotationSpeed();
	// ターゲット方向に回転
	_owner->GetEnemy()->LookAtTarget(targetPosition, elapsedTime, rotationSpeed);

	// 現在の向いている方向に移動
	Vector3 velocity = _owner->GetEnemy()->GetActor()->GetTransform().GetAxisZ().Normalize() * 
		_owner->GetWyvern()->GetFlightMoveSpeed();
	velocity.y = 0.0f;
	_owner->GetEnemy()->GetCharactorController()->SetVelocity(
		velocity
	);

	// アニメーションが終了しているとき滞空状態へ遷移
	if (!_owner->GetAnimator()->IsPlayAnimation())
		_owner->GetBase().ChangeState("HoverIdle");
}

void WyvernHoverToTargetState::OnExit()
{
	// 移動停止
	_owner->GetEnemy()->GetCharactorController()->SetVelocity(Vector3::Zero);
}
#pragma endregion

#pragma region 空中回転
namespace WyvernHoverTurnSubStates
{
	class TurnSubStateBase : public StateBase<WyvernStateMachine>
	{
	public:
		TurnSubStateBase(WyvernStateMachine* owner,
			const std::string& name,
			const std::string& animationName) :
			StateBase(owner),
			_name(name),
			_animationName(animationName)
		{
		}
		const char* GetName() const override { return _name.c_str(); }
		void OnEnter() override
		{
			_owner->GetAnimator()->PlayAnimation(
				_animationName,
				false,
				1.5f);
			_owner->GetAnimator()->SetIsUseRootMotion(true);
			_owner->GetAnimator()->SetIsRemoveRootMovement(true);
		}
		void OnExecute(float elapsedTime) override {}
		void OnExit() override {}
	private:
		std::string _name;
		std::string _animationName;
	};
}
WyvernHoverTurnState::WyvernHoverTurnState(WyvernStateMachine* owner) : HierarchicalStateBase(owner)
{
	// サブステートを追加
	RegisterSubState(std::make_unique<WyvernHoverTurnSubStates::TurnSubStateBase>(owner, u8"TurnLeft90", u8"DodgeFlyLeft"));
	RegisterSubState(std::make_unique<WyvernHoverTurnSubStates::TurnSubStateBase>(owner, u8"TurnRight90", u8"DodgeFlyRight"));
}
void WyvernHoverTurnState::OnEnter()
{
	_rootNodeIndex = _owner->GetEnemy()->GetActor()->GetModel().lock()->GetNodeIndex("CG");

	auto& position = _owner->GetEnemy()->GetActor()->GetTransform().GetPosition();
	auto& targetPosition = _owner->GetCombatStatus()->GetTargetPosition();
	auto targetDirection = (targetPosition - position).Normalize();
	auto front = _owner->GetEnemy()->GetActor()->GetTransform().GetAxisZ().Normalize();
	// ターゲット位置からどの方向に回転するか判定
	float crossY = front.Cross(targetDirection).y;

	std::string subStateName = "TurnLeft90";
	if (crossY > 0.0f)
		subStateName = "TurnRight90";

	// サブステート設定
	ChangeSubState(subStateName);
	_owner->GetAnimator()->SetIsUseRootMotion(true);
}

void WyvernHoverTurnState::OnExecute(float elapsedTime)
{
	// アニメーションが終了しているとき滞空状態へ遷移
	if (!_owner->GetAnimator()->IsPlayAnimation())
		_owner->GetBase().ChangeState("HoverIdle");
}

void WyvernHoverTurnState::OnExit()
{
	// 現在のアニメーションの回転量を取り除き、アクターの回転に反映する
	// 回転量の差分を求める
	Quaternion q = _owner->GetAnimator()->RemoveRootRotation(_rootNodeIndex);

	// 回転量をアクターに反映する
	auto& transform = _owner->GetEnemy()->GetActor()->GetTransform();
	Vector3 angle{};
	// y値をyに設定
	angle.y = -q.ToRollPitchYaw().y;
	transform.AddRotation(angle);
	transform.UpdateTransform(nullptr);
}
#pragma endregion

#pragma region 空中火球
void WyvernHoverFireBallAttackState::OnEnter()
{
	_fireBallActor.reset();
	_owner->GetAnimator()->PlayAnimation(
		u8"AttackFlyFireBall",
		false,
		1.5f);
	_owner->GetAnimator()->SetIsUseRootMotion(false);
}

void WyvernHoverFireBallAttackState::OnExecute(float elapsedTime)
{
	// 火球処理
	if (_owner->CallFireBallEvent())
	{
		// 火球の生成
		if (!_fireBallActor.lock())
		{
			// 火球のグローバル座標取得
			Vector3 fireBallGlobalPosition = _owner->GetFireBallGlobalPosition();
			// 火球を生成
			_fireBallActor = _owner->GetEnemy()->GetActor()->GetScene()->RegisterActor<WyvernBallActor>(
				std::string(_owner->GetEnemy()->GetActor()->GetName()) + "FireBallEffect",
				ActorTag::Enemy);
			// 親を設定
			_fireBallActor.lock()->GetBallController()->SetBallActor(_owner->GetEnemy()->GetActor());
			Vector3 position = Vector3::TransformCoord(fireBallGlobalPosition, _owner->GetEnemy()->GetActor()->GetTransform().GetMatrix());
			_fireBallActor.lock()->GetTransform().SetPosition(position);
			// ブレスのアクターを頭の向いている方向に向かせる
			_fireBallActor.lock()->GetTransform().SetAngleY(_owner->GetEnemy()->GetActor()->GetTransform().GetRotation().y);
		}
	}
	else
	{
		auto& position = _owner->GetEnemy()->GetActor()->GetTransform().GetPosition();
		auto& targetPosition = _owner->GetCombatStatus()->GetTargetPosition();
		auto targetDirection = (targetPosition - position);
		float rotationSpeed = _owner->GetEnemy()->GetRotationSpeed() * 1.5f/*TODO : 変数化*/;
		// ターゲット方向に回転
		_owner->GetEnemy()->LookAtTarget(targetPosition, elapsedTime, rotationSpeed);
	}
	// アニメーションが終了しているとき滞空状態へ遷移
	if (!_owner->GetAnimator()->IsPlayAnimation())
		_owner->GetBase().ChangeState("HoverIdle");
}

void WyvernHoverFireBallAttackState::OnExit()
{
}
#pragma endregion

#pragma region 空中かぎ爪攻撃
void WyvernHoverClawAttackState::OnEnter()
{
	_owner->GetAnimator()->PlayAnimation(
		u8"AttackFlyLegs",
		false,
		1.5f);
	_owner->GetAnimator()->SetIsUseRootMotion(false);
}

void WyvernHoverClawAttackState::OnExecute(float elapsedTime)
{
	// アニメーションが終了しているとき滞空状態へ遷移
	if (!_owner->GetAnimator()->IsPlayAnimation())
		_owner->GetBase().ChangeState("HoverIdle");
}

void WyvernHoverClawAttackState::OnExit()
{
}
#pragma endregion

#pragma region 墜落
namespace WyvernHitFallSubState
{
	class WyvernHitFallStartState : public WyvernSSB
	{
	public:
		WyvernHitFallStartState(WyvernStateMachine* owner) :
			WyvernSSB(
				owner,
				u8"HitFallStart",
				u8"FallHitDown",
				1.5f,
				false,
				false
			) {
		}
		void OnExecute(float elapsedTime) override
		{
			// アニメーションが終了しているとき次の状態へ遷移
			if (!_owner->GetAnimator()->IsPlayAnimation())
			{
				_owner->GetBase().ChangeSubState(u8"HitFallLoop");
			}
		}
	};
	class WyvernHitFallLoopState : public WyvernSSB
	{
	public:
		WyvernHitFallLoopState(WyvernStateMachine* owner) :
			WyvernSSB(
				owner,
				u8"HitFallLoop",
				u8"FallHitLoop",
				1.5f,
				true,
				false
			) {
		}
		void OnExecute(float elapsedTime) override
		{
			if (_owner->GetEnemy()->GetCharactorController()->GetVelocity().y >= 0)
			{
				_owner->GetBase().ChangeSubState(u8"HitFallEnd");
			}
		}
	};
	class WyvernHitFallEndState : public WyvernSSB
	{
	public:
		WyvernHitFallEndState(WyvernStateMachine* owner) :
			WyvernSSB(
				owner,
				u8"HitFallEnd",
				u8"FallDeathGround",
				1.5f,
				false,
				false
			) {
		}
		void OnExecute(float elapsedTime) override
		{
			// アニメーションが終了しているとき次の状態へ遷移
			if (!_owner->GetAnimator()->IsPlayAnimation())
			{
				_owner->ChangeState(u8"Down", u8"DownRLoop");
			}
		}
	};
}
WyvernHitFallState::WyvernHitFallState(WyvernStateMachine* owner) : 
	HierarchicalStateBase(owner)
{
	RegisterSubState(std::make_unique<WyvernHitFallSubState::WyvernHitFallStartState>(owner));
	RegisterSubState(std::make_unique<WyvernHitFallSubState::WyvernHitFallLoopState>(owner));
	RegisterSubState(std::make_unique<WyvernHitFallSubState::WyvernHitFallEndState>(owner));
}
void WyvernHitFallState::OnEnter()
{
	// サブステートを開始状態に設定
	ChangeSubState(u8"HitFallStart");
}

void WyvernHitFallState::OnExecute(float elapsedTime)
{
}

void WyvernHitFallState::OnExit()
{
}
#pragma endregion

#pragma region 着地
void WyvernLandState::OnEnter()
{
	_owner->GetAnimator()->PlayAnimation(
		u8"FallLand",
		false,
		1.5f);
	_owner->GetAnimator()->SetIsUseRootMotion(false);
}

void WyvernLandState::OnExecute(float elapsedTime)
{
	// アニメーションが終了しているとき待機状態へ遷移
	if (!_owner->GetAnimator()->IsPlayAnimation())
		_owner->GetBase().ChangeState("Idle");
}

void WyvernLandState::OnExit()
{
}
#pragma endregion
