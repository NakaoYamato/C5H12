#include "WyvernStateController.h"

#include "../../Library/Component/Animator.h"
#include "../WyvernController.h"
#include "WyvernMainStates.h"
#include "WyvernHoverState.h"

#include <imgui.h>

// 開始時処理
void WyvernStateController::OnStart()
{
	// コンポーネント取得
	_enemy = GetActor()->GetComponent<EnemyController>().get();
	_wyvern = GetActor()->GetComponent<WyvernController>().get();
	_animator = GetActor()->GetComponent<Animator>().get();
	_damageable = GetActor()->GetComponent<Damageable>().get();
	_damageSender = GetActor()->GetComponent<DamageSender>().get();
	_combatStatus = GetActor()->GetComponent<CombatStatusController>().get();
	_roarController = GetActor()->GetComponent<RoarController>().get();

	// ステート設定
	_stateMachine->RegisterState(std::make_unique<WyvernIdleState>(this));
	_stateMachine->RegisterState(std::make_unique<WyvernThreatState>(this));

	_stateMachine->RegisterState(std::make_unique<WyvernRoarState>(this));
	_stateMachine->RegisterState(std::make_unique<WyvernToTargetState>(this));
	_stateMachine->RegisterState(std::make_unique<WyvernTurnState>(this));

	_stateMachine->RegisterState(std::make_unique<WyvernBiteAttackState>(this));
	_stateMachine->RegisterState(std::make_unique<WyvernClawAttackState>(this));
	_stateMachine->RegisterState(std::make_unique<WyvernTailAttackState>(this));
	_stateMachine->RegisterState(std::make_unique<WyvernChargeAttackState>(this));
	_stateMachine->RegisterState(std::make_unique<WyvernBreathAttackState>(this));
	_stateMachine->RegisterState(std::make_unique<WyvernFireBallAttackState>(this));

	_stateMachine->RegisterState(std::make_unique<WyvernBackJumpFireBallAttackState>(this));

	_stateMachine->RegisterState(std::make_unique<WyvernBackStepState>(this));
	_stateMachine->RegisterState(std::make_unique<WyvernPursuitState>(this));

	_stateMachine->RegisterState(std::make_unique<WyvernDamageState>(this));
	_stateMachine->RegisterState(std::make_unique<WyvernDownState>(this));
	_stateMachine->RegisterState(std::make_unique<WyvernDeathState>(this));

	_stateMachine->RegisterState(std::make_unique<WyvernHoverIdleState>(this));
	_stateMachine->RegisterState(std::make_unique<WyvernHoverToTargetState>(this));
	_stateMachine->RegisterState(std::make_unique<WyvernHoverTurnState>(this));
	_stateMachine->RegisterState(std::make_unique<WyvernHoverFireBallAttackState>(this));
	_stateMachine->RegisterState(std::make_unique<WyvernHoverClawAttackState>(this));
	_stateMachine->RegisterState(std::make_unique<WyvernHitFallState>(this));
	_stateMachine->RegisterState(std::make_unique<WyvernLandState>(this));


	// ルートモーション設定
	GetAnimator()->SetRootNodeIndex("CG");
	GetAnimator()->SetRootMotionOption(Animator::RootMotionOption::RemovePositionX);
	GetAnimator()->SetRootOffset(Vector3(0.0f, 200.0f, 0.0f));

	// 初期ステート設定
	ChangeState("Idle");
}

// 遅延更新処理
void WyvernStateController::OnLateUpdate(float elapsedTime)
{
	float blurRate = 0.0f;
	_callCancelEvent = false;
	_callFireBreath = false;
	_callFireBall = false;
	_callLookAtTarget = false;

	// アニメーションイベント取得
	if (GetAnimator()->IsPlaying())
	{
		auto& animationEvent = GetAnimator()->GetAnimationEvent();
		int massageListSize = (int)animationEvent.GetMessageList().size();
		auto events = GetAnimator()->GetCurrentEvents();
		for (auto& event : events)
		{
			if (event.eventType != AnimationEvent::EventType::Flag)
				continue;

			// メッセージインデックスが範囲外ならcontinue
			if (event.messageIndex < 0 || event.messageIndex >= massageListSize)
				continue;

			auto& message = animationEvent.GetMessageList().at(event.messageIndex);

			// キャンセル判定
			if (message == "Cancel")
			{
				_callCancelEvent = true;
			}
			// ブレス攻撃判定
			else if (message == "FireBreath")
			{
				_callFireBreath = true;
				// ブレス攻撃のグローバル位置を設定
				_breathGlobalPosition = event.position;
			}
			// 火球攻撃判定
			else if (message == "FireBall")
			{
				_callFireBall = true;
				// 火球攻撃のグローバル位置を設定
				_fireBallGlobalPosition = event.position;
			}
			// ルックアット判定
			else if (message == "LookAtTarget")
			{
				_callLookAtTarget = true;
			}
			// 咆哮判定
			else if (message == "Roar")
			{
				blurRate = 1.0f - (GetAnimator()->GetAnimationTimer() - event.startSeconds) / (event.endSeconds - event.startSeconds);
			}
			// 飛行開始判定
			else if (message == "StartFlight")
			{
				GetWyvern()->SetIsDuringFlight(true);
			}
			// 飛行終了判定
			else if (message == "EndFlight")
			{
				GetWyvern()->SetIsDuringFlight(false);
			}
		}
	}

	// 咆哮のブラー処理
	if (blurRate > 0.0f)
	{
		// ワイバーンの頭の位置を取得
		auto model = GetEnemy()->GetActor()->GetModel().lock();
		int nodeIndex = model->GetNodeIndex("Head");
		Vector3 headWorldPosition =
		{
			model->GetPoseNodes()[nodeIndex].worldTransform._41,
			model->GetPoseNodes()[nodeIndex].worldTransform._42,
			model->GetPoseNodes()[nodeIndex].worldTransform._43
		};
		_roarController->SetWorldPosition(headWorldPosition);
		_roarController->SetRate(blurRate);
	}
	else
	{
		_roarController->Reset();
	}
}

// GUI描画
void WyvernStateController::OnDrawGui()
{
	if (auto stateMachine = GetStateMachine())
	{
		std::string str = u8"現在のステート:";
		str += stateMachine->GetStateName();
		ImGui::Text("%s", str.c_str());
		str = u8"現在のサブステート:";
		str += stateMachine->GetSubStateName();
		ImGui::Text("%s", str.c_str());
	}
	ImGui::Separator();
}

#pragma region ベースステート
void WyvernHSB::OnEnter()
{
	_rootNodeIndex = _owner->GetEnemy()->GetActor()->GetModel().lock()->GetNodeIndex("CG");
	_owner->GetAnimator()->PlayAnimation(_animationName, _isLoop, _blendSeconds);
	_owner->GetAnimator()->SetIsUseRootMotion(_isUsingRootMotion);
}

void WyvernHSB::Exit()
{
	if (_applyRotation)
	{
		// 現在のアニメーションの回転量を取り除き、アクターの回転に反映する
		// 回転量の差分を求める
		Quaternion q = _owner->GetAnimator()->RemoveRootRotation(_rootNodeIndex);

		// 回転量をアクターに反映する
		auto& transform = _owner->GetEnemy()->GetActor()->GetTransform();
		Vector3 angle{};
		// y値をyに設定
		angle.y = -q.ToRollPitchYaw().y;
		transform.AddAngle(angle);
		transform.UpdateTransform(nullptr);
	}
}

void WyvernSSB::OnEnter()
{
	_rootNodeIndex = _owner->GetEnemy()->GetActor()->GetModel().lock()->GetNodeIndex("CG");
	_owner->GetAnimator()->SetIsUseRootMotion(_isUsingRootMotion);
	_owner->GetAnimator()->PlayAnimation(_animationName, _isLoop, _blendSeconds);
}
void WyvernSSB::OnExit()
{
	if (_applyRotation)
	{
		// 現在のアニメーションの回転量を取り除き、アクターの回転に反映する
		// 回転量の差分を求める
		Quaternion q = _owner->GetAnimator()->RemoveRootRotation(_rootNodeIndex);

		// 回転量をアクターに反映する
		auto& transform = _owner->GetEnemy()->GetActor()->GetTransform();
		Vector3 angle{};
		// y値をyに設定
		angle.y = -q.ToRollPitchYaw().y;
		transform.AddAngle(angle);
		transform.UpdateTransform(nullptr);
	}
}
#pragma endregion
