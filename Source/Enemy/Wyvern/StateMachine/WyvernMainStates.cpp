#include "WyvernMainStates.h"

#include "../../Library/Component/Animator.h"
#include "../../Library/Scene/Scene.h"
#include "../WyvernEnemyController.h"
#include "../../Library/DebugSupporter/DebugSupporter.h"

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

#pragma region ターゲットに向かう
namespace WyvernToTargetSubStates
{
	class WalkForwardSubState : public StateBase<WyvernStateMachine>
	{
	public:
		WalkForwardSubState(WyvernStateMachine* owner) : StateBase(owner) {}
		const char* GetName() const override { return "WalkForward"; }
		void OnEnter() override
		{
			owner->GetAnimator()->PlayAnimation(
				u8"WalkForward",
				true,
				1.0f);
			owner->GetAnimator()->SetRootNodeIndex("CG");
			owner->GetAnimator()->SetIsUseRootMotion(true);
			owner->GetAnimator()->SetRootMotionOption(Animator::RootMotionOption::RemovePositionXY);
		}
		void OnExecute(float elapsedTime) override 
		{
			auto& targetPosition = owner->GetWyvern()->GetTargetPosition();
			float rotationSpeed = owner->GetWyvern()->GetRotationSpeed();
			// ターゲット方向に回転
			owner->GetWyvern()->LookAtTarget(targetPosition, elapsedTime, rotationSpeed);

			// ターゲット方向に向いているか判定
			float angleToTarget = owner->GetWyvern()->GetAngleToTarget(targetPosition);
			if (angleToTarget < owner->GetWyvern()->GetLookAtRadian())
			{
				// ターゲットに到達したら待機状態へ遷移
				owner->GetStateMachine().ChangeState("Idle");
			}
		}
		void OnExit() override {}
	};
	class TurnLeft90SubState : public StateBase<WyvernStateMachine>
	{
	public:
		TurnLeft90SubState(WyvernStateMachine* owner) : StateBase(owner) {}
		const char* GetName() const override { return "TurnLeft90"; }
		void OnEnter() override
		{
			owner->GetAnimator()->PlayAnimation(
				u8"TurnLeft90",
				false,
				0.2f);
			owner->GetAnimator()->SetRootNodeIndex("CG");
			owner->GetAnimator()->SetIsUseRootMotion(true);
			owner->GetAnimator()->SetRootMotionOption(Animator::RootMotionOption::RemovePositionXY);
			owner->GetAnimator()->SetIsRemoveRootMovement(true);
		}
		void OnExecute(float elapsedTime) override 
		{
			// アニメーションが終了しているとき
			if (!owner->GetAnimator()->IsPlayAnimation())
			{
				// 歩行状態に移行
				owner->GetStateMachine().ChangeSubState("WalkForward");
			}
		}
		void OnExit() override
		{
			// 現在のアニメーションの回転量を取り除き、アクターの回転に反映する
			int rootNodeIndex = owner->GetWyvern()->GetActor()->GetModel().lock()->GetNodeIndex("CG");
			// 回転量の差分を求める
			Quaternion q = owner->GetAnimator()->RemoveRootRotation(rootNodeIndex);

			// 回転量をアクターに反映する
			auto& transform = owner->GetWyvern()->GetActor()->GetTransform();
			Vector3 angle{};
			// ルートの行列の都合上z値をyに設定
			angle.y = q.ToRollPitchYaw().z;
			transform.AddRotation(angle);

			// フラグを下ろす
			owner->GetAnimator()->SetIsRemoveRootMovement(false);
		}
	};
	class TurnLeft180SubState : public StateBase<WyvernStateMachine>
	{
	public:
		TurnLeft180SubState(WyvernStateMachine* owner) : StateBase(owner) {}
		const char* GetName() const override { return "TurnLeft180"; }
		void OnEnter() override
		{
			owner->GetAnimator()->PlayAnimation(
				u8"TurnLeft180",
				false,
				0.2f);
			owner->GetAnimator()->SetRootNodeIndex("CG");
			owner->GetAnimator()->SetIsUseRootMotion(true);
			owner->GetAnimator()->SetRootMotionOption(Animator::RootMotionOption::RemovePositionXY);
			owner->GetAnimator()->SetIsRemoveRootMovement(true);
		}
		void OnExecute(float elapsedTime) override
		{
			// アニメーションが終了しているとき
			if (!owner->GetAnimator()->IsPlayAnimation())
			{
				// 歩行状態に移行
				owner->GetStateMachine().ChangeSubState("WalkForward");
			}
		}
		void OnExit() override
		{
			// 現在のアニメーションの回転量を取り除き、アクターの回転に反映する
			int rootNodeIndex = owner->GetWyvern()->GetActor()->GetModel().lock()->GetNodeIndex("CG");
			// 回転量の差分を求める
			Quaternion q = owner->GetAnimator()->RemoveRootRotation(rootNodeIndex);

			// 回転量をアクターに反映する
			auto& transform = owner->GetWyvern()->GetActor()->GetTransform();
			Vector3 angle{};
			// ルートの行列の都合上z値をyに設定
			angle.y = q.ToRollPitchYaw().z;
			transform.AddRotation(angle);

			// フラグを下ろす
			owner->GetAnimator()->SetIsRemoveRootMovement(false);
		}
	};
	class TurnRight90SubState : public StateBase<WyvernStateMachine>
	{
	public:
		TurnRight90SubState(WyvernStateMachine* owner) : StateBase(owner) {}
		const char* GetName() const override { return "TurnRight90"; }
		void OnEnter() override
		{
			owner->GetAnimator()->PlayAnimation(
				u8"TurnRight90",
				false,
				0.2f);
			owner->GetAnimator()->SetRootNodeIndex("CG");
			owner->GetAnimator()->SetIsUseRootMotion(true);
			owner->GetAnimator()->SetRootMotionOption(Animator::RootMotionOption::RemovePositionXY);
			owner->GetAnimator()->SetIsRemoveRootMovement(true);
		}
		void OnExecute(float elapsedTime) override
		{
			// アニメーションが終了しているとき
			if (!owner->GetAnimator()->IsPlayAnimation())
			{
				// 歩行状態に移行
				owner->GetStateMachine().ChangeSubState("WalkForward");
			}
		}
		void OnExit() override
		{
			// 現在のアニメーションの回転量を取り除き、アクターの回転に反映する
			int rootNodeIndex = owner->GetWyvern()->GetActor()->GetModel().lock()->GetNodeIndex("CG");
			// 回転量の差分を求める
			Quaternion q = owner->GetAnimator()->RemoveRootRotation(rootNodeIndex);
			// 回転量をアクターに反映する
			auto& transform = owner->GetWyvern()->GetActor()->GetTransform();
			Vector3 angle{};
			// ルートの行列の都合上z値をyに設定
			angle.y = q.ToRollPitchYaw().z;
			transform.AddRotation(angle);
			// フラグを下ろす
			owner->GetAnimator()->SetIsRemoveRootMovement(false);
		}
	};
	class TurnRight180SubState : public StateBase<WyvernStateMachine>
	{
	public:
		TurnRight180SubState(WyvernStateMachine* owner) : StateBase(owner) {}
		const char* GetName() const override { return "TurnRight180"; }
		void OnEnter() override
		{
			owner->GetAnimator()->PlayAnimation(
				u8"TurnRight180",
				false,
				0.2f);
			owner->GetAnimator()->SetRootNodeIndex("CG");
			owner->GetAnimator()->SetIsUseRootMotion(true);
			owner->GetAnimator()->SetRootMotionOption(Animator::RootMotionOption::RemovePositionXY);
			owner->GetAnimator()->SetIsRemoveRootMovement(true);
		}
		void OnExecute(float elapsedTime) override
		{
			// アニメーションが終了しているとき
			if (!owner->GetAnimator()->IsPlayAnimation())
			{
				// 歩行状態に移行
				owner->GetStateMachine().ChangeSubState("WalkForward");
			}
		}
		void OnExit() override
		{
			// 現在のアニメーションの回転量を取り除き、アクターの回転に反映する
			int rootNodeIndex = owner->GetWyvern()->GetActor()->GetModel().lock()->GetNodeIndex("CG");
			// 回転量の差分を求める
			Quaternion q = owner->GetAnimator()->RemoveRootRotation(rootNodeIndex);
			// 回転量をアクターに反映する
			auto& transform = owner->GetWyvern()->GetActor()->GetTransform();
			Vector3 angle{};
			// ルートの行列の都合上z値をyに設定
			angle.y = q.ToRollPitchYaw().z;
			transform.AddRotation(angle);
			// フラグを下ろす
			owner->GetAnimator()->SetIsRemoveRootMovement(false);
		}
	};
}
WyvernToTargetState::WyvernToTargetState(WyvernStateMachine* owner) :
	HierarchicalStateBase(owner)
{
	// サブステートを追加
	RegisterSubState(std::make_unique<WyvernToTargetSubStates::WalkForwardSubState>(owner));
	RegisterSubState(std::make_unique<WyvernToTargetSubStates::TurnLeft90SubState>(owner));
	RegisterSubState(std::make_unique<WyvernToTargetSubStates::TurnLeft180SubState>(owner));
	RegisterSubState(std::make_unique<WyvernToTargetSubStates::TurnRight90SubState>(owner));
	RegisterSubState(std::make_unique<WyvernToTargetSubStates::TurnRight180SubState>(owner));
}
void WyvernToTargetState::OnEnter()
{
	auto& position = owner->GetWyvern()->GetActor()->GetTransform().GetPosition();
	auto& targetPosition = owner->GetWyvern()->GetTargetPosition();
	auto targetDirection = (targetPosition - position).Normalize();
	auto front = owner->GetWyvern()->GetActor()->GetTransform().GetAxisZ().Normalize();
	// ターゲット位置からどの方向に回転するか判定
	float crossY = front.Cross(targetDirection).y;
	float dot = front.Dot(targetDirection);

	std::string subStateName = "WalkForward";
	if (crossY > 0.0f)
	{
		// ターゲット位置がワイバーンから見て右方向にある
		// 90度以上角度があるか確認
		if (dot < 0.0f)
		{
			// 一定量以上角度がある場合は180度回転
 			if (dot < -0.5f)
			{
				// 右に180度回転
				subStateName = "TurnRight180";
			}
			else
			{
				// 右に90度回転
				subStateName = "TurnRight90";
			}
		}
	}
	else
	{
		// ターゲット位置がワイバーンから見て左方向にある
		// 90度以上角度があるか確認
		if (dot < 0.0f)
		{
			// 一定量以上角度がある場合は180度回転
			if (dot < -0.5f)
			{
				// 左に180度回転
				subStateName = "TurnLeft180";
			}
			else
			{
				// 左に90度回転
				subStateName = "TurnLeft90";
			}
		}
	}
	// サブステート設定
	ChangeSubState(subStateName);
}

void WyvernToTargetState::OnExecute(float elapsedTime)
{
}

void WyvernToTargetState::OnExit()
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

		// 手のノードインデックスを取得
		auto model = owner->GetWyvern()->GetActor()->GetModel().lock();
		_handNodeIndex = model->GetNodeIndex("R Hand");
		// ターゲット位置設定
		DirectX::XMMATRIX WyvernTransform = DirectX::XMLoadFloat4x4(&owner->GetWyvern()->GetActor()->GetTransform().GetMatrix());
		Vector3 targetLocalPosition = targetPosition.TransformCoord(DirectX::XMMatrixInverse(nullptr, WyvernTransform));
		targetLocalPosition = targetLocalPosition.ClampSphere(Vector3(
			TargetLocalLimitPositionX,
			0.0f, 
			TargetLocalLimitPositionZ),
			TargetLocalLimitRadius);
		targetLocalPosition.y = 0.0f; // y座標は0にする
		_targetWorldPosition = targetLocalPosition.TransformCoord(WyvernTransform);
	}
	else
	{
		// 左に攻撃
		// アニメーション再生
		owner->GetAnimator()->PlayAnimation(
			u8"AttackWingFistLeft",
			false,
			0.5f);

		// 手のノードインデックスを取得
		auto model = owner->GetWyvern()->GetActor()->GetModel().lock();
		_handNodeIndex = model->GetNodeIndex("L Hand");
		// ターゲット位置設定
		DirectX::XMMATRIX WyvernTransform = DirectX::XMLoadFloat4x4(&owner->GetWyvern()->GetActor()->GetTransform().GetMatrix());
		Vector3 targetLocalPosition = targetPosition.TransformCoord(DirectX::XMMatrixInverse(nullptr, WyvernTransform));
		targetLocalPosition = targetLocalPosition.ClampSphere(Vector3(
			-TargetLocalLimitPositionX,
			0.0f,
			TargetLocalLimitPositionZ),
			TargetLocalLimitRadius);
		targetLocalPosition.y = 0.0f; // y座標は0にする
		_targetWorldPosition = targetLocalPosition.TransformCoord(WyvernTransform);
	}
	owner->GetAnimator()->SetRootNodeIndex("CG");
	owner->GetAnimator()->SetIsUseRootMotion(true);
	owner->GetAnimator()->SetRootMotionOption(Animator::RootMotionOption::RemovePositionXY);

	_lerpTimer = 0.0f;
}
void WyvernClawAttackState::OnExecute(float elapsedTime)
{
	// 手の位置をターゲット位置に設定
	float animationTimer = owner->GetAnimator()->GetAnimationTimer();
	if (animationTimer > 1.7f)
	{
		_lerpTimer -= elapsedTime * _endLerpSpeed;
	}
	else if (animationTimer >= 1.2f)
	{
		_lerpTimer += elapsedTime * _startLerpSpeed;
	}
	if (_lerpTimer > 0.0f)
	{
		// モデル情報、手のノード情報、手の親ノード情報を取得
		auto model = owner->GetWyvern()->GetActor()->GetModel().lock();
		auto& handNode = model->GetPoseNodes()[_handNodeIndex];
		auto armNord = handNode.parent;

		// 現在の腕のアニメーション回転量を取得
		Quaternion animationMidRotation = armNord->rotation;
		// 腕ノードを初期姿勢に戻す
		armNord->rotation = Quaternion::Identity;
		model->UpdateNodeTransform(armNord);

		// 腕ノードをターゲット方向に向ける
		DirectX::XMMATRIX ArmTransform = DirectX::XMLoadFloat4x4(&armNord->worldTransform);
		DirectX::XMMATRIX HandTransform = DirectX::XMLoadFloat4x4(&handNode.worldTransform);
		DirectX::XMVECTOR TargetWorldPosition = DirectX::XMLoadFloat3(&_targetWorldPosition);
		DirectX::XMVECTOR ArmPosition = ArmTransform.r[3];
		DirectX::XMVECTOR HandPosition = HandTransform.r[3];
		// 腕ノードからターゲットへのベクトル
		DirectX::XMVECTOR ArmToTargetNormal = DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(TargetWorldPosition, ArmPosition));
		// 腕ノードから手ノードへのベクトル
		DirectX::XMVECTOR ArmToHandNormal = DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(HandPosition, ArmPosition));
		// 腕ノードからターゲットへのベクトルと腕ノードから手ノードへのベクトルで腕ノードの回転角と回転軸を作る
		DirectX::XMVECTOR ArmWorldAxis = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(ArmToHandNormal, ArmToTargetNormal));
		float ArmAngle = acosf(DirectX::XMVectorGetX(DirectX::XMVector3Dot(ArmToTargetNormal, ArmToHandNormal)));

		// 求めた回転軸をローカル座標変換
		DirectX::XMVECTOR ArmLocalAxis = DirectX::XMVector4Transform(ArmWorldAxis, DirectX::XMMatrixInverse(nullptr, ArmTransform));
		DirectX::XMVECTOR OldArmLocalRotation = DirectX::XMLoadFloat4(&armNord->rotation);
		// ローカル回転軸と回転角を使ってQuaternionを作成
		ArmLocalAxis = DirectX::XMQuaternionMultiply(OldArmLocalRotation, DirectX::XMQuaternionRotationAxis(ArmLocalAxis, ArmAngle));
		Quaternion nextRotation{};
		DirectX::XMStoreFloat4(&nextRotation, DirectX::XMQuaternionNormalize(ArmLocalAxis));
		// 補間処理
		armNord->rotation = Quaternion::Slerp(
			animationMidRotation,
			nextRotation,
			MathF::Clamp01(_lerpTimer));

		// 自分以下の行列を更新
		model->UpdateNodeTransform(armNord);
	}

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
	auto& targetPosition = owner->GetWyvern()->GetTargetPosition();
	float rotationSpeed = owner->GetWyvern()->GetRotationSpeed();
	// ターゲット方向に回転
	owner->GetWyvern()->LookAtTarget(targetPosition, elapsedTime, rotationSpeed);

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
	// ブレス処理
	if (owner->CallFireBreathEvent())
	{
		// ブレスの位置をモデルの頭に設定
		auto model = owner->GetWyvern()->GetActor()->GetModel().lock();
		int nodeIndex = model->GetNodeIndex("Head");
		// ブレスの生成
		if (!_fireBreathActor.lock())
		{
			// ブレスを生成
			_fireBreathActor = owner->GetWyvern()->GetActor()->GetScene()->RegisterActor<WyvernBreathActor>(
				std::string(owner->GetWyvern()->GetActor()->GetName()) + "BreathEffect",
				ActorTag::Enemy);
			// 親を設定
			_fireBreathActor.lock()->GetBreathController()->SetBreathActor(owner->GetWyvern()->GetActor());
			Vector3 position = Vector3::TransformCoord(_fireBreathOffset + model->GetPoseNodes()[nodeIndex].position,
				model->GetPoseNodes()[nodeIndex].worldTransform);
			_fireBreathActor.lock()->GetTransform().SetPosition(position);
			// ブレスのアクターを頭の向いている方向に向かせる
			_fireBreathActor.lock()->GetTransform().LookAt(Vector3::TransformNormal(Vector3::Up, model->GetPoseNodes()[nodeIndex].worldTransform));
		}
	}
	// ブレスの削除
	if (!owner->CallFireBreathEvent() && _fireBreathActor.lock())
	{
		_fireBreathActor.lock()->Remove();
	}

	// アニメーションが終了しているとき
	if (!owner->GetAnimator()->IsPlayAnimation())
	{
		// 待機状態へ遷移
		owner->GetStateMachine().ChangeState("Idle");
	}
}
void WyvernBreathAttackState::OnExit()
{
	// ブレスのエフェクトを削除
	if (_fireBreathActor.lock())
	{
		_fireBreathActor.lock()->Remove();
	}

	owner->GetAnimator()->SetIsUseRootMotion(false);
}
#pragma endregion

#pragma region 近づく
void WyvernPursuitState::OnEnter()
{
	owner->GetAnimator()->PlayAnimation(
		u8"RunForward",
		true,
		1.0f);
	owner->GetAnimator()->SetRootNodeIndex("CG");
	owner->GetAnimator()->SetIsUseRootMotion(true);
	owner->GetAnimator()->SetRootMotionOption(Animator::RootMotionOption::RemovePositionXY);
}
void WyvernPursuitState::OnExecute(float elapsedTime)
{
	auto& position = owner->GetWyvern()->GetActor()->GetTransform().GetPosition();
	auto& targetPosition = owner->GetWyvern()->GetTargetPosition();
	auto targetDirection = (targetPosition - position);
	float rotationSpeed = owner->GetWyvern()->GetRotationSpeed();
	// ターゲット方向に回転
	owner->GetWyvern()->LookAtTarget(targetPosition, elapsedTime, rotationSpeed);
	
	// 現在の位置とターゲットの位置の距離から攻撃できるか判定
	float nearAttackRange = owner->GetWyvern()->GetNearAttackRange();
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
	auto hitPosition = owner->GetDamageable()->GetHitPosition();
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
	// アニメーションが終了しているとき
	if (!owner->GetAnimator()->IsPlayAnimation())
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
		// 回転量の差分を求める
		Quaternion q = owner->GetAnimator()->RemoveRootRotation(rootNodeIndex);

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
