#include "WyvernMainStates.h"

#include "../../Library/Component/Animator.h"
#include "../../Library/Scene/Scene.h"
#include "../WyvernController.h"

#include "../../Source/Camera/HuntingSuccessCamera.h"

#include <imgui.h>

#pragma region 待機
void WyvernIdleState::OnExecute(float elapsedTime)
{
	// アニメーションが終了しているときステートの終了
	if (!_owner->GetAnimator()->IsPlaying())
	{
		_owner->GetBase().EndState();
	}
}
#pragma endregion

#pragma region 威嚇
void WyvernThreatState::OnExecute(float elapsedTime)
{
	// アニメーションが終了しているときステートの終了
	if (!_owner->GetAnimator()->IsPlaying())
	{
		_owner->GetBase().EndState();
	}
}
#pragma endregion

#pragma region 咆哮
// 実行処理
void WyvernRoarState::OnExecute(float elapsedTime)
{
	// アニメーションが終了しているときステートの終了
	if (!_owner->GetAnimator()->IsPlaying())
	{
		_owner->GetBase().EndState();
	}
}
#pragma endregion

#pragma region ターゲットに向く
WyvernTurnState::WyvernTurnState(WyvernStateMachine* owner) :
	HierarchicalStateBase(owner)
{
	// サブステートを追加
	RegisterSubState(std::make_unique<WyvernSSB>(owner, u8"TurnLeft90", u8"TurnLeft90", 1.5f, false, true, true));
	RegisterSubState(std::make_unique<WyvernSSB>(owner, u8"TurnLeft180", u8"TurnLeft180", 1.5f, false, true, true));
	RegisterSubState(std::make_unique<WyvernSSB>(owner, u8"TurnRight90", u8"TurnRight90", 1.5f, false, true, true));
	RegisterSubState(std::make_unique<WyvernSSB>(owner, u8"TurnRight180", u8"TurnRight180", 1.5f, false, true, true));
}

void WyvernTurnState::OnEnter()
{
	_owner->GetAnimator()->SetIsRemoveRootMovement(true);

	auto& position = _owner->GetEnemy()->GetActor()->GetTransform().GetPosition();
	auto& targetPosition = _owner->GetCombatStatus()->GetTargetPosition();
	auto targetDirection = (targetPosition - position).Normalize();
	auto front = _owner->GetEnemy()->GetActor()->GetTransform().GetAxisZ().Normalize();
	// ターゲット位置からどの方向に回転するか判定
	float crossY = front.Cross(targetDirection).y;
	float dot = front.Dot(targetDirection);

	std::string subStateName = "TurnLeft90";
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

void WyvernTurnState::OnExecute(float elapsedTime)
{
	// アニメーションが終了しているときステートの終了
	if (!_owner->GetAnimator()->IsPlaying())
	{
		_owner->GetBase().EndState();
	}
}

void WyvernTurnState::OnExit()
{
	// フラグを下ろす
	_owner->GetAnimator()->SetIsRemoveRootMovement(false);
}
#pragma endregion

#pragma region ターゲットに近づく
void WyvernToTargetState::OnExecute(float elapsedTime)
{
	auto& targetPosition = _owner->GetCombatStatus()->GetTargetPosition();
	float rotationSpeed = _owner->GetEnemy()->GetRotationSpeed();
	// ターゲット方向に回転
	_owner->GetEnemy()->LookAtTarget(targetPosition, elapsedTime, rotationSpeed);

	// ターゲット方向に向いているか判定
	float angle = _owner->GetEnemy()->GetAngleToTarget(targetPosition);
	// 近接攻撃範囲内に入ったらステートの終了
	float attackRange = _owner->GetEnemy()->GetNearAttackRange();
	float length = _owner->GetCombatStatus()->GetToTargetVec().Length();
	if (length < attackRange && angle < _owner->GetEnemy()->GetLookAtRadian() / 2.0f)
	{
		_owner->GetBase().EndState();
		return;
	}
    // 極近距離ならステートの終了
	if (length < attackRange / 2.0f)
	{
		_owner->GetBase().EndState();
        return;
	}

	// アニメーションが終了しているときステートの終了
	if (!_owner->GetAnimator()->IsPlaying())
	{
		_owner->GetBase().EndState();
	}
}
#pragma endregion

#pragma region 噛みつき攻撃
WyvernBiteAttackState::WyvernBiteAttackState(WyvernStateMachine* owner) : 
	HierarchicalStateBase(owner)
{
	// サブステートを追加
	RegisterSubState(std::make_unique<WyvernSSB>(owner, u8"AttackBiteLeft", u8"AttackBiteLeft", 1.5f, false, false, true));
	RegisterSubState(std::make_unique<WyvernSSB>(owner, u8"AttackBiteRight", u8"AttackBiteRight", 1.5f, false, false, true));
}
void WyvernBiteAttackState::OnEnter()
{
	// 攻撃力設定
	_owner->GetDamageSender()->SetMotionFactor(MotionAttackFactor);

	// ターゲット座標から左前右のどこに攻撃するか判定
	auto& position = _owner->GetEnemy()->GetActor()->GetTransform().GetPosition();
	auto& targetPosition = _owner->GetCombatStatus()->GetTargetPosition();
	auto targetDirection = (targetPosition - position).Normalize();
	auto front = _owner->GetEnemy()->GetActor()->GetTransform().GetAxisZ().Normalize();
	float crossY = front.Cross(targetDirection).y;
	if (crossY > 0.0f)
	{
		// 右に攻撃
		ChangeSubState(u8"AttackBiteLeft");
	}
	else
	{
		// 左に攻撃
		ChangeSubState(u8"AttackBiteRight");
	}
}
void WyvernBiteAttackState::OnExecute(float elapsedTime)
{
	// アニメーションが終了しているときステートの終了
	if (!_owner->GetAnimator()->IsPlaying())
	{
		_owner->GetBase().EndState();
	}
}
void WyvernBiteAttackState::OnExit()
{
}
#pragma endregion

#pragma region かぎ爪攻撃
WyvernClawAttackState::WyvernClawAttackState(WyvernStateMachine* owner) :
	HierarchicalStateBase(owner)
{
	// サブステートを追加
	RegisterSubState(std::make_unique<WyvernSSB>(owner, u8"AttackWingFistLeft", u8"AttackWingFistLeft", 1.5f, false, false, true));
	RegisterSubState(std::make_unique<WyvernSSB>(owner, u8"AttackWingFistRight", u8"AttackWingFistRight", 1.5f, false, false, true));
}
void WyvernClawAttackState::OnEnter()
{
	// 攻撃力設定
	_owner->GetDamageSender()->SetMotionFactor(MotionAttackFactor);

	auto model = _owner->GetEnemy()->GetActor()->GetModel().lock();
	// ターゲット座標から左前右のどこに攻撃するか判定
	auto& position = _owner->GetEnemy()->GetActor()->GetTransform().GetPosition();
	auto& targetPosition = _owner->GetCombatStatus()->GetTargetPosition();
	auto targetDirection = (targetPosition - position).Normalize();
	auto front = _owner->GetEnemy()->GetActor()->GetTransform().GetAxisZ().Normalize();
	float crossY = front.Cross(targetDirection).y;
	if (crossY > 0.0f)
	{
		// 右に攻撃
		ChangeSubState(u8"AttackWingFistRight");

		// 手のノードインデックスを取得
		_handNodeIndex = model->GetNodeIndex("R Hand");
		// ターゲット位置設定
		DirectX::XMMATRIX WyvernTransform = DirectX::XMLoadFloat4x4(&_owner->GetEnemy()->GetActor()->GetTransform().GetMatrix());
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
		ChangeSubState(u8"AttackWingFistLeft");

		// 手のノードインデックスを取得
		_handNodeIndex = model->GetNodeIndex("L Hand");
		// ターゲット位置設定
		DirectX::XMMATRIX WyvernTransform = DirectX::XMLoadFloat4x4(&_owner->GetEnemy()->GetActor()->GetTransform().GetMatrix());
		Vector3 targetLocalPosition = targetPosition.TransformCoord(DirectX::XMMatrixInverse(nullptr, WyvernTransform));
		targetLocalPosition = targetLocalPosition.ClampSphere(Vector3(
			-TargetLocalLimitPositionX,
			0.0f,
			TargetLocalLimitPositionZ),
			TargetLocalLimitRadius);
		targetLocalPosition.y = 0.0f; // y座標は0にする
		_targetWorldPosition = targetLocalPosition.TransformCoord(WyvernTransform);
	}
	_owner->GetAnimator()->SetIsUseRootMotion(true);

	_lerpTimer = 0.0f;
}
void WyvernClawAttackState::OnExecute(float elapsedTime)
{
	// 手の位置をターゲット位置に設定
	float animationTimer = _owner->GetAnimator()->GetAnimationTimer();
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
		auto model = _owner->GetEnemy()->GetActor()->GetModel().lock();
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

	// アニメーションが終了しているときステートの終了
	if (!_owner->GetAnimator()->IsPlaying())
	{
		_owner->GetBase().EndState();
	}
}
void WyvernClawAttackState::OnExit()
{
}
#pragma endregion
#pragma region 尻尾攻撃
WyvernTailAttackState::WyvernTailAttackState(WyvernStateMachine* owner) :
	HierarchicalStateBase(owner)
{
	// サブステートを追加
	RegisterSubState(std::make_unique<WyvernSSB>(owner, u8"AttackTailLeft", u8"AttackTailLeft", 1.5f, false, false, true));
	RegisterSubState(std::make_unique<WyvernSSB>(owner, u8"AttackTailRight", u8"AttackTailRight", 1.5f, false, false, true));
}
void WyvernTailAttackState::OnEnter()
{
	// 攻撃力設定
	_owner->GetDamageSender()->SetMotionFactor(MotionAttackFactor);

	// ターゲット座標から左前右のどこに攻撃するか判定
	auto& position = _owner->GetEnemy()->GetActor()->GetTransform().GetPosition();
	auto& targetPosition = _owner->GetCombatStatus()->GetTargetPosition();
	auto targetDirection = (targetPosition - position).Normalize();
	auto front = _owner->GetEnemy()->GetActor()->GetTransform().GetAxisZ().Normalize();
	float crossY = front.Cross(targetDirection).y;
	if (crossY > 0.0f)
	{
		// 右に攻撃
		// アニメーション再生
		ChangeSubState(u8"AttackTailRight");
	}
	else
	{
		// 左に攻撃
		// アニメーション再生
		ChangeSubState(u8"AttackTailLeft");
	}
}
void WyvernTailAttackState::OnExecute(float elapsedTime)
{
	// アニメーションが終了しているときステートの終了
	if (!_owner->GetAnimator()->IsPlaying())
	{
		_owner->GetBase().EndState();
	}
}
void WyvernTailAttackState::OnExit()
{
}
#pragma endregion

#pragma region 突進攻撃
void WyvernChargeAttackState::OnEnter()
{
	WyvernHSB::OnEnter();
	_owner->GetAnimator()->SetRootMotionOption(Animator::RootMotionOption::UseOffset);
	_startPosition = _owner->GetEnemy()->GetActor()->GetTransform().GetPosition();
	// 攻撃力設定
	_owner->GetDamageSender()->SetMotionFactor(MotionAttackFactor);
}

void WyvernChargeAttackState::OnExecute(float elapsedTime)
{
	// アニメーションが終了しているとき
	if (!_owner->GetAnimator()->IsPlaying())
	{
		// 現在の位置がターゲット位置を越しているか判定
		auto& currentPosition = _owner->GetEnemy()->GetActor()->GetTransform().GetPosition();
		auto& targetPosition = _owner->GetCombatStatus()->GetTargetPosition();
		auto toTargetVecLenSq	= (targetPosition - _startPosition).LengthSq();
		auto toCurrentVecLenSq	= (currentPosition - _startPosition).LengthSq();
		if (toCurrentVecLenSq >= toTargetVecLenSq)
		{
			// ターゲット位置を越しているならステートの終了
			_owner->GetBase().EndState();
			return;
		}
		else
		{
			// 越していないなら再度突進攻撃を行う
			_owner->GetAnimator()->PlayAnimation(
				u8"AttackCharge",
				false,
				1.0f);
			return;
		}
	}
}

void WyvernChargeAttackState::OnExit()
{
	_owner->GetAnimator()->SetRootMotionOption(Animator::RootMotionOption::RemovePositionX);
}
#pragma endregion

#pragma region 後退
void WyvernBackStepState::OnExecute(float elapsedTime)
{
	auto& targetPosition = _owner->GetCombatStatus()->GetTargetPosition();
	float rotationSpeed = _owner->GetEnemy()->GetRotationSpeed();
	// ターゲット方向に回転
	_owner->GetEnemy()->LookAtTarget(targetPosition, elapsedTime, rotationSpeed);

	// アニメーションが終了しているときステートの終了
	if (!_owner->GetAnimator()->IsPlaying())
	{
		_owner->GetBase().EndState();
	}
}
#pragma endregion

#pragma region ブレス
void WyvernBreathAttackState::OnExecute(float elapsedTime)
{
	// ブレス処理
	if (_owner->CallFireBreathEvent())
	{
		// ブレスの位置をモデルの頭に設定
		auto model = _owner->GetEnemy()->GetActor()->GetModel().lock();
		int nodeIndex = model->GetNodeIndex("Head");
		// ブレスの生成
		if (!_fireBreathActor.lock())
		{
			// ブレスのグローバル座標取得
			Vector3 breathGlobalPosition = _owner->GetBreathGlobalPosition();
			// ブレスを生成
			_fireBreathActor = _owner->GetEnemy()->GetActor()->GetScene()->RegisterActor<WyvernBreathActor>(
				std::string(_owner->GetEnemy()->GetActor()->GetName()) + "BreathEffect",
				ActorTag::Enemy);
			// 親を設定
			_fireBreathActor.lock()->GetBreathController()->SetBreathActor(_owner->GetEnemy()->GetActor());
			Vector3 position = Vector3::TransformCoord(breathGlobalPosition, _owner->GetEnemy()->GetActor()->GetTransform().GetMatrix());
			_fireBreathActor.lock()->GetTransform().SetPosition(position);
			// ブレスのアクターを頭の向いている方向に向かせる
			_fireBreathActor.lock()->GetTransform().SetAngleY(_owner->GetEnemy()->GetActor()->GetTransform().GetAngle().y);
		}
	}
	// ブレスの削除
	if (!_owner->CallFireBreathEvent() && _fireBreathActor.lock())
	{
		_fireBreathActor.lock()->Remove();
	}

	// アニメーションが終了しているときステートの終了
	if (!_owner->GetAnimator()->IsPlaying())
	{
		_owner->GetBase().EndState();
	}
}
void WyvernBreathAttackState::OnExit()
{
	// ブレスのエフェクトを削除
	_fireBreathActor.reset();
}
#pragma endregion

#pragma region 火球
void WyvernFireBallAttackState::OnExecute(float elapsedTime)
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
			_fireBallActor.lock()->GetTransform().SetAngleY(_owner->GetEnemy()->GetActor()->GetTransform().GetAngle().y);
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

	// アニメーションが終了しているときステートの終了
	if (!_owner->GetAnimator()->IsPlaying())
	{
		_owner->GetBase().EndState();
	}
}

void WyvernFireBallAttackState::OnExit()
{
	_fireBallActor.reset();
	_owner->GetAnimator()->SetIsUseRootMotion(false);
}
#pragma endregion

#pragma region バックジャンプ火球
void WyvernBackJumpFireBallAttackState::OnExecute(float elapsedTime)
{
	// 火球処理
	if (_owner->CallFireBallEvent())
	{
		// 火球の生成
		if (!_fireBallActor.lock())
		{
			// ワイバーンの頭の位置を取得
			auto model = _owner->GetEnemy()->GetActor()->GetModel().lock();
			int nodeIndex = model->GetNodeIndex("Head");
			DirectX::XMFLOAT4X4 worldMatrix = model->GetPoseNodes()[nodeIndex].worldTransform;
			Vector3 headWorldPosition =
			{
				worldMatrix._41,
				worldMatrix._42,
				worldMatrix._43
			};
			// 火球を生成
			_fireBallActor = _owner->GetEnemy()->GetActor()->GetScene()->RegisterActor<WyvernBallActor>(
				std::string(_owner->GetEnemy()->GetActor()->GetName()) + "FireBallEffect",
				ActorTag::Enemy);
			// 親を設定
			_fireBallActor.lock()->GetBallController()->SetBallActor(_owner->GetEnemy()->GetActor());
			_fireBallActor.lock()->GetTransform().SetPosition(headWorldPosition);
			// ブレスのアクターを頭の向いている方向に向かせる
			_fireBallActor.lock()->GetTransform().SetAngleY(_owner->GetEnemy()->GetActor()->GetTransform().GetAngle().y);
			_fireBallActor.lock()->GetTransform().SetAngleX(_launchAngleX);
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
	// アニメーションが終了しているときステートの終了
	if (!_owner->GetAnimator()->IsPlaying())
	{
		_owner->GetBase().EndState();
	}
}

void WyvernBackJumpFireBallAttackState::OnExit()
{
	_fireBallActor.reset();
}
#pragma endregion

#pragma region 近づく
void WyvernPursuitState::OnEnter()
{
	_owner->GetAnimator()->PlayAnimation(
		u8"RunForward",
		true,
		1.0f);
	_owner->GetAnimator()->SetIsUseRootMotion(true);
	_owner->GetAnimator()->SetRootMotionOption(Animator::RootMotionOption::UseOffset);
}
void WyvernPursuitState::OnExecute(float elapsedTime)
{
	auto& position = _owner->GetEnemy()->GetActor()->GetTransform().GetPosition();
	auto& targetPosition = _owner->GetCombatStatus()->GetTargetPosition();
	auto targetDirection = (targetPosition - position);
	float rotationSpeed = _owner->GetEnemy()->GetRotationSpeed();
	// ターゲット方向に回転
	_owner->GetEnemy()->LookAtTarget(targetPosition, elapsedTime, rotationSpeed);
	
	// 現在の位置とターゲットの位置の距離から攻撃できるか判定
	float nearAttackRange = _owner->GetEnemy()->GetNearAttackRange();
	if (targetDirection.Length() < nearAttackRange)
	{
		// 攻撃範囲内ならステートの終了
		_owner->GetBase().EndState();
		return;
	}
}
void WyvernPursuitState::OnExit()
{
	_owner->GetAnimator()->SetIsUseRootMotion(false);
	_owner->GetAnimator()->SetRootMotionOption(Animator::RootMotionOption::RemovePositionX);
}
#pragma endregion

#pragma region ダメージを受ける
WyvernDamageState::WyvernDamageState(WyvernStateMachine* owner) :
	HierarchicalStateBase(owner)
{
	// サブステートを追加
	RegisterSubState(std::make_unique<WyvernSSB>(owner, u8"DamageFrontRight", u8"DamageFrontRight", 1.5f, false, false, true));
	RegisterSubState(std::make_unique<WyvernSSB>(owner, u8"DamageRight", u8"DamageRight", 1.5f, false, false, true));
	RegisterSubState(std::make_unique<WyvernSSB>(owner, u8"DamageBackRight", u8"DamageBackRight", 1.5f, false, false, true));
	RegisterSubState(std::make_unique<WyvernSSB>(owner, u8"DamageFrontLeft", u8"DamageFrontLeft", 1.5f, false, false, true));
	RegisterSubState(std::make_unique<WyvernSSB>(owner, u8"DamageLeft", u8"DamageLeft", 1.5f, false, false, true));
	RegisterSubState(std::make_unique<WyvernSSB>(owner, u8"DamageBackLeft", u8"DamageBackLeft", 1.5f, false, false, true));
}
void WyvernDamageState::OnEnter()
{
	_owner->GetAnimator()->SetIsRemoveRootMovement(true);
	// BehaviorTreeで遷移を制御
}
void WyvernDamageState::OnExecute(float elapsedTime)
{
	// アニメーションが終了しているときステートの終了
	if (!_owner->GetAnimator()->IsPlaying())
	{
		_owner->GetBase().EndState();
	}
}
void WyvernDamageState::OnExit()
{
	_owner->GetAnimator()->SetIsRemoveRootMovement(false);
}
#pragma endregion

#pragma region ダウン
namespace WyvernDownSubState
{
	class WyvernDownStartState : public WyvernSSB
	{
	public:
		WyvernDownStartState(WyvernStateMachine* owner,
			const std::string& animationName,
			const std::string& nextState) :
			WyvernSSB(owner, animationName, animationName, 1.5f, false, false),
			_nextState(nextState)
		{
		}
		void OnExecute(float elapsedTime) override
		{
			// アニメーションが終了しているとき次の状態へ遷移
			if (!_owner->GetAnimator()->IsPlaying())
			{
				_owner->GetBase().ChangeSubState(_nextState);
			}
		}
	private:
		std::string _nextState;
	};
	class WyvernDownLoopState : public WyvernSSB
	{
	public:
		WyvernDownLoopState(WyvernStateMachine* owner,
			const std::string& animationName,
			const std::string& nextState,
			float timer) :
			WyvernSSB(owner, animationName, animationName, 1.0f, true, false),
			_nextState(nextState),
			_time(timer),
			_timer(0.0f)
		{
		}
		void OnEnter() override
		{
			WyvernSSB::OnEnter();
			_timer = _time;
		}
		void OnExecute(float elapsedTime) override
		{
			_timer -= elapsedTime;
			// タイマーが0以下になったら次の状態へ遷移
			if (_timer <= 0.0f)
			{
				_owner->GetBase().ChangeSubState(_nextState);
			}
		}
	private:
		std::string _nextState;
		float _timer;
		float _time;
	};
	class WyvernDownEndState : public WyvernSSB
	{
	public:
		WyvernDownEndState(WyvernStateMachine* owner,
			const std::string& animationName) :
			WyvernSSB(owner, animationName, animationName, 1.5f, false, false)
		{
		}
		void OnExecute(float elapsedTime) override
		{
			// アニメーションが終了しているときステートの終了
			if (!_owner->GetAnimator()->IsPlaying())
			{
				_owner->GetBase().EndState();
			}
		}
	};
}
WyvernDownState::WyvernDownState(WyvernStateMachine* owner) :
	HierarchicalStateBase(owner)
{
	// サブステートを追加
	RegisterSubState(std::make_unique<WyvernDownSubState::WyvernDownStartState>(owner, u8"DownRStart", u8"DownRLoop"));
	RegisterSubState(std::make_unique<WyvernDownSubState::WyvernDownLoopState>(owner, u8"DownRLoop", u8"DownREnd", _downTime));
	RegisterSubState(std::make_unique<WyvernDownSubState::WyvernDownEndState>(owner, u8"DownREnd"));

	RegisterSubState(std::make_unique<WyvernDownSubState::WyvernDownStartState>(owner, u8"DownLStart", u8"DownLLoop"));
	RegisterSubState(std::make_unique<WyvernDownSubState::WyvernDownLoopState>(owner, u8"DownLLoop", u8"DownLEnd", _downTime));
	RegisterSubState(std::make_unique<WyvernDownSubState::WyvernDownEndState>(owner, u8"DownLEnd"));
}
void WyvernDownState::OnEnter()
{
	// BehaviorTreeで遷移を制御
}
void WyvernDownState::OnExit()
{
}
#pragma endregion

#pragma region 死亡
void WyvernDeathState::OnEnter()
{
	_owner->GetAnimator()->PlayAnimation(
		u8"Death01",
		false,
		0.5f);
	_owner->GetAnimator()->SetIsUseRootMotion(true);

    // ハンティング成功カメラを起動
	for (auto& child : _owner->GetEnemy()->GetActor()->GetScene()->GetMainCameraActor()->GetChildren())
	{
		auto huntingSuccessCamera = child->GetComponent<HuntingSuccessCamera>();
		if (huntingSuccessCamera)
		{
			huntingSuccessCamera->Swich();
            huntingSuccessCamera->SetTarget(
                _owner->GetEnemy()->GetActor()->GetTransform().GetPosition(),
                _owner->GetEnemy()->GetActor()->GetTransform().GetAxisZ(),
				10.0f
			);
			break;
		}
    }

}
void WyvernDeathState::OnExecute(float elapsedTime)
{
}
void WyvernDeathState::OnExit()
{
	_owner->GetAnimator()->SetIsUseRootMotion(false);
}
#pragma endregion

#pragma region 位置調整
WyvernPositionAdjustState::WyvernPositionAdjustState(WyvernStateMachine* owner) : 
	HierarchicalStateBase(owner)
{
	// サブステートを追加
	RegisterSubState(std::make_unique<WyvernSSB>(owner, u8"WalkLeft", u8"WalkLeft", 1.5f, false, true, true));
	RegisterSubState(std::make_unique<WyvernSSB>(owner, u8"WalkRight", u8"WalkRight", 1.5f, false, true, true));
}

void WyvernPositionAdjustState::OnEnter()
{
}

void WyvernPositionAdjustState::OnExecute(float elapsedTime)
{
}

void WyvernPositionAdjustState::OnExit()
{
}

#pragma endregion
