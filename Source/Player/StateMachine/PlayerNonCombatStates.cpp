#include "PlayerNonCombatStates.h"

#include "../../Library/Component/Animator.h"
#include "../PlayerController.h"
#include "../../Library/Algorithm/Converter.h"

#define NON_TURN

#pragma region 待機
void PlayerNonCombatIdleState::OnExecute(float elapsedTime)
{
	// 抜刀遷移
	if (_owner->GetPlayer()->IsAttack())
	{
		// 移動しながらか、最後の移動からの経過時間が一定値以下なら抜刀攻撃
		if (_owner->GetPlayer()->IsMoving() ||
			_owner->GetPlayer()->GetTimeSinceLastMove() <= 0.4f)
			_owner->GetStateMachine().ChangeState("CombatAttack1");
		else
			_owner->GetStateMachine().ChangeState("ToCombat");
	}
	// 移動
	else if (_owner->GetPlayer()->IsMoving())
		_owner->GetStateMachine().ChangeState("Walk");
	// 回避移行
	else if (_owner->GetPlayer()->IsEvade())
		_owner->GetStateMachine().ChangeState("Evade");
	// ガード移行
	else if (_owner->GetPlayer()->IsGuard())
		_owner->GetStateMachine().ChangeState("CombatGuard");
	// アイテム使用
	else if (_owner->GetPlayer()->IsUsingItem())
		_owner->ChangeItemState();
}
#pragma endregion

#pragma region 振り向き
PlayerNonCombatTurnState::PlayerNonCombatTurnState(PlayerStateMachine* stateMachine) :
	HierarchicalStateBase(stateMachine)
{
	RegisterSubState(std::make_shared<PlayerSSB>(stateMachine, "TurnL", "Turn180L", 0.5f, false, true));
	RegisterSubState(std::make_shared<PlayerSSB>(stateMachine, "TurnR", "Turn180R", 0.5f, false, true));
}

void PlayerNonCombatTurnState::OnEnter()
{
	_owner->GetAnimator()->SetIsRemoveRootMovement(true);
	Vector2 movement = _owner->GetPlayer()->GetMovement();
	// 入力方向とプレイヤーのY軸回転量から角度取得
	float angle =
		DirectX::XMConvertToDegrees(
			atan2f(movement.x, movement.y)
			- _owner->GetPlayer()->GetActor()->GetTransform().GetAngle().y
		);
	// 角度を0~360度に正規化
	angle = fmodf(angle, 360.0f);
	if (angle < 0.0f)
		angle += 360.0f;

	if (angle < 180.0f)
		_owner->GetStateMachine().ChangeSubState("TurnR");
	else
		_owner->GetStateMachine().ChangeSubState("TurnL");
}

void PlayerNonCombatTurnState::OnExecute(float elapsedTime)
{
	// アニメーションが終了しているとき
	if (!_owner->GetAnimator()->IsPlaying())
	{
		// 待機に移行
		_owner->GetStateMachine().ChangeState("Idle");
	}
}

void PlayerNonCombatTurnState::OnExit()
{
	// 現在のアニメーションの回転量を取り除き、アクターの回転に反映する
	int rootNodeIndex = _owner->GetPlayer()->GetActor()->GetModel().lock()->GetNodeIndex("root");
	// 回転量の差分を求める
	Quaternion q = _owner->GetAnimator()->RemoveRootRotation(rootNodeIndex);
	// 回転量をアクターに反映する
	auto& transform = _owner->GetPlayer()->GetActor()->GetTransform();
	Vector3 angle{};
	// z値をyに設定
	angle.y = q.ToRollPitchYaw().z;
	transform.AddAngle(angle);
	transform.UpdateTransform(nullptr);
	_owner->GetAnimator()->SetIsRemoveRootMovement(false);
}
#pragma endregion

#pragma region 歩き
namespace NonCombatWalkSubState
{
	// 歩き開始
    class WalkStartSubState : public PlayerSSB
    {
    public:
		WalkStartSubState(PlayerStateMachine* stateMachine) :
			PlayerSSB(stateMachine,
				"WalkStart",
                u8"RunStartF0", 0.2f, 
                false, 
                true) 
        {
		}
		~WalkStartSubState() override {}
		void OnExecute(float elapsedTime) override
		{
#ifndef NON_TURN
			// プレイヤーの正面と移動方向から振り向きが必要か判定
			bool isNeedTurn = false;
			Vector2 movement = _owner->GetPlayer()->GetMovement();
			if (movement.LengthSq() != 0.0f)
			{
				// 入力方向とプレイヤーのY軸回転量から角度取得
				float angle =
					DirectX::XMConvertToDegrees(
						atan2f(movement.x, movement.y)
						- _owner->GetPlayer()->GetActor()->GetTransform().GetAngle().y
					);
				// 角度を0~360度に正規化
				angle = fmodf(angle, 360.0f);
				if (angle < 0.0f)
					angle += 360.0f;

				if (angle > 135.0f && angle <= 225.0f)
				{
					// 後ろに回転
					_owner->GetStateMachine().ChangeState("Turn");
					return;
				}
			}
#endif // !NON_TURN

			// 移動方向に向く
			_owner->RotationMovement(elapsedTime);
			// アニメーションが終了していたら遷移
			if (!_owner->GetAnimator()->IsPlaying())
				_owner->GetStateMachine().ChangeSubState("Walking");
			// 移動していなければ終了に遷移
			if (!_owner->GetPlayer()->IsMoving())
				_owner->GetStateMachine().ChangeSubState("WalkStop");
		}
    };
	// 歩きループ
	class WalkingSubState : public PlayerSSB
	{
	public:
		WalkingSubState(PlayerStateMachine* stateMachine) :
			PlayerSSB(stateMachine,
				"Walking",
				u8"RunLoopF0", 0.2f,
				true,
				true)
		{
		}
		~WalkingSubState() override {}
		void OnExecute(float elapsedTime) override
		{
			// 移動方向に向く
			_owner->RotationMovement(elapsedTime);
			// 移動していなければ終了に遷移
			if (!_owner->GetPlayer()->IsMoving())
				_owner->GetStateMachine().ChangeSubState("WalkStop");
#ifndef NON_TURN
			// プレイヤーの正面と移動方向から振り向きが必要か判定
			Vector2 movement = _owner->GetPlayer()->GetMovement();
			if (movement.LengthSq() != 0.0f)
			{
				// 入力方向とプレイヤーのY軸回転量から角度取得
				float angle =
					DirectX::XMConvertToDegrees(
						atan2f(movement.x, movement.y)
						- _owner->GetPlayer()->GetActor()->GetTransform().GetAngle().y
					);
				// 角度を0~360度に正規化
				angle = fmodf(angle, 360.0f);
				if (angle < 0.0f)
					angle += 360.0f;

				if (angle > 135.0f && angle <= 225.0f)
				{
					// 後ろに回転
					_owner->GetStateMachine().ChangeState("Turn");
					return;
				}
			}
#endif // !NON_TURN
		}
	};
	// 歩き停止
	class WalkStopSubState : public PlayerSSB
	{
	public:
		WalkStopSubState(PlayerStateMachine* stateMachine) :
			PlayerSSB(stateMachine,
				"WalkStop",
				u8"RunStopF0", 0.2f,
				false,
				true)
		{
		}
		~WalkStopSubState() override {}
		void OnExecute(float elapsedTime) override
		{
			// 移動方向に向く
			_owner->RotationMovement(elapsedTime);
			// 移動があれば遷移
			if (_owner->GetPlayer()->IsMoving())
				_owner->GetStateMachine().ChangeSubState("Walking");
			// アニメーションが終了していたら遷移
			else if (!_owner->GetAnimator()->IsPlaying())
				_owner->GetStateMachine().ChangeState("Idle");
		}
	};
}
PlayerNonCombatWalkState::PlayerNonCombatWalkState(PlayerStateMachine* stateMachine) : 
	HierarchicalStateBase(stateMachine)
{
	RegisterSubState(std::make_shared<NonCombatWalkSubState::WalkStartSubState>(stateMachine));
	RegisterSubState(std::make_shared<NonCombatWalkSubState::WalkingSubState>(stateMachine));
	RegisterSubState(std::make_shared<NonCombatWalkSubState::WalkStopSubState>(stateMachine));
}
void PlayerNonCombatWalkState::OnEnter()
{
	// フラグを立てる
	// TODO : ネットワークの処理修正
	//_owner->GetPlayer()->SetIsMoving(true);
	ChangeSubState("WalkStart");
}

void PlayerNonCombatWalkState::OnExecute(float elapsedTime)
{
	// 走り始めではなく、移動があって、ダッシュ入力があれば走りへ遷移
	if (_owner->GetPlayer()->IsMoving() && _owner->GetPlayer()->IsDash())
	{
		if (std::string(this->GetSubStateName()) != "WalkStart")
		{
			_owner->GetStateMachine().ChangeState("Run");
			_owner->GetStateMachine().ChangeSubState("Running");
		}
	}

	// 回避移行
	if (_owner->GetPlayer()->IsEvade())
		_owner->GetStateMachine().ChangeState("Evade");
	// ガード移行
	else if (_owner->GetPlayer()->IsGuard())
		_owner->GetStateMachine().ChangeState("CombatGuard");
	// 攻撃移行
	else if (_owner->GetPlayer()->IsAttack())
	{
		// 移動しながらか、最後の移動からの経過時間が一定値以下なら抜刀攻撃
		if (_owner->GetPlayer()->IsMoving() ||
			_owner->GetPlayer()->GetTimeSinceLastMove() <= 0.4f)
			_owner->GetStateMachine().ChangeState("CombatAttack1");
		// 移動していなければ抜刀
		else
			_owner->GetStateMachine().ChangeState("ToCombat");
	}
	// アイテム使用
	else if (_owner->GetPlayer()->IsUsingItem())
		_owner->ChangeItemState();
}
void PlayerNonCombatWalkState::OnExit()
{
	// フラグをおろす
	// TODO : ネットワークの処理修正
	_owner->GetPlayer()->SetIsMoving(false);
}
#pragma endregion

#pragma region 走り
namespace NonCombatRunSubState
{
	// 走り開始
	class RunStartSubState : public PlayerSSB
	{
	public:
		RunStartSubState(PlayerStateMachine* stateMachine) :
			PlayerSSB(stateMachine,
				"RunStart",
				u8"RunStartF0", 0.2f,
				false,
				true)
		{
		}
		~RunStartSubState() override {}
		void OnExecute(float elapsedTime) override
		{
			// 移動方向に向く
			_owner->RotationMovement(_owner->GetPlayer()->GetDashRotationFactor() * elapsedTime);
			// アニメーションが終了していたら遷移
			if (!_owner->GetAnimator()->IsPlaying())
				_owner->GetStateMachine().ChangeSubState("Running");
			// 移動していなければ終了に遷移
			else if (!_owner->GetPlayer()->IsMoving())
				_owner->GetStateMachine().ChangeSubState("RunStop");
		}
	};
	// 走りループ
	class RunningSubState : public PlayerSSB
	{
	public:
		RunningSubState(PlayerStateMachine* stateMachine) :
			PlayerSSB(stateMachine,
				"Running",
				u8"RunFastLoop", 1.0f,
				true,
				true)
		{
		}
		~RunningSubState() override {}
		void OnEnter() override
		{
			PlayerSSB::OnEnter();
			_timer = 0.0f;
		}
		void OnExecute(float elapsedTime) override
		{
			// 移動方向に向く
			_owner->RotationMovement(_owner->GetPlayer()->GetDashRotationFactor() * elapsedTime);
			// 入力が一定時間ないなら終了に遷移
			if (!_owner->GetPlayer()->IsMoving())
			{
				_timer += elapsedTime;
				if (_timer >= _runningTime)
					_owner->GetStateMachine().ChangeSubState("RunStop");
			}
			else
			{
				_timer = 0.0f;
			}
			// プレイヤーの正面と移動方向から振り向きが必要か判定
			bool isNeedTurn = false;
			Vector2 movement = _owner->GetPlayer()->GetMovement();
			if (movement.LengthSq() != 0.0f)
			{
				// 入力方向とプレイヤーのY軸回転量から角度取得
				float angle =
					DirectX::XMConvertToDegrees(
						atan2f(movement.x, movement.y)
						- _owner->GetPlayer()->GetActor()->GetTransform().GetAngle().y
					);
				// 角度を0~360度に正規化
				angle = fmodf(angle, 360.0f);
				if (angle < 0.0f)
					angle += 360.0f;

				if (180.0f > angle && angle > 135.0f)
				{
					// 後ろに回転
					_owner->GetStateMachine().ChangeSubState("RunTurnL");
					return;
				}
				else if (180.0f <= angle && angle < 225.0f)
				{
					// 後ろに回転
					_owner->GetStateMachine().ChangeSubState("RunTurnR");
					return;
				}
			}
		}
	private:
		float _timer = 0.0f;
		float _runningTime = 0.2f;
	};
	// 走り停止
	class RunStopSubState : public PlayerSSB
	{
	public:
		RunStopSubState(PlayerStateMachine* stateMachine) :
			PlayerSSB(stateMachine,
				"RunStop",
				u8"RunFastStop", 0.5f,
				false,
				true)
		{
		}
		~RunStopSubState() override {}
		void OnExecute(float elapsedTime) override
		{
			// 移動方向に向く
			_owner->RotationMovement(_owner->GetPlayer()->GetDashRotationFactor() * elapsedTime);
			// 移動があれば遷移
			if (_owner->GetPlayer()->IsMoving())
				_owner->GetStateMachine().ChangeSubState("Running");
			// アニメーションが終了していたら遷移
			else if (!_owner->GetAnimator()->IsPlaying())
				_owner->GetStateMachine().ChangeState("Idle");
		}
	};
	// 走りながら回転L
	class RunTurnLSubState : public PlayerSSB
	{
	public:
		RunTurnLSubState(PlayerStateMachine* stateMachine) :
			PlayerSSB(stateMachine,
				"RunTurnL",
				u8"RunFastTurnL", 0.2f,
				false,
				true)
		{
		}
		~RunTurnLSubState() override {}
		void OnExecute(float elapsedTime) override
		{
			// アニメーションが終了していたら遷移
			if (!_owner->GetAnimator()->IsPlaying())
				_owner->GetStateMachine().ChangeSubState("Running");
		}
		void OnExit() override
		{
			// 現在のアニメーションの回転量を取り除き、アクターの回転に反映する
			int rootNodeIndex = _owner->GetPlayer()->GetActor()->GetModel().lock()->GetNodeIndex("root");
			// 回転量の差分を求める
			Quaternion q = _owner->GetAnimator()->RemoveRootRotation(rootNodeIndex);
			// 回転量をアクターに反映する
			auto& transform = _owner->GetPlayer()->GetActor()->GetTransform();
			Vector3 angle{};
			// z値をyに設定
			angle.y = q.ToRollPitchYaw().z;
			transform.AddAngle(angle);
			transform.UpdateTransform(nullptr);
		}
	};
	// 走りながら回転R
	class RunTurnRSubState : public PlayerSSB
	{
	public:
		RunTurnRSubState(PlayerStateMachine* stateMachine) :
			PlayerSSB(stateMachine,
				"RunTurnR",
				u8"RunFastTurnR", 0.2f,
				false,
				true)
		{
		}
		~RunTurnRSubState() override {}
		void OnExecute(float elapsedTime) override
		{
			// アニメーションが終了していたら遷移
			if (!_owner->GetAnimator()->IsPlaying())
				_owner->GetStateMachine().ChangeSubState("Running");
		}
		void OnExit() override
		{
			// 現在のアニメーションの回転量を取り除き、アクターの回転に反映する
			int rootNodeIndex = _owner->GetPlayer()->GetActor()->GetModel().lock()->GetNodeIndex("root");
			// 回転量の差分を求める
			Quaternion q = _owner->GetAnimator()->RemoveRootRotation(rootNodeIndex);
			// 回転量をアクターに反映する
			auto& transform = _owner->GetPlayer()->GetActor()->GetTransform();
			Vector3 angle{};
			// z値をyに設定
			angle.y = q.ToRollPitchYaw().z;
			transform.AddAngle(angle);
			transform.UpdateTransform(nullptr);
		}
	};
}
PlayerNonCombatRunState::PlayerNonCombatRunState(PlayerStateMachine* stateMachine) : 
	HierarchicalStateBase(stateMachine)
{
	RegisterSubState(std::make_shared<NonCombatRunSubState::RunStartSubState>(stateMachine));
	RegisterSubState(std::make_shared<NonCombatRunSubState::RunningSubState>(stateMachine));
	RegisterSubState(std::make_shared<NonCombatRunSubState::RunStopSubState>(stateMachine));
	RegisterSubState(std::make_shared<NonCombatRunSubState::RunTurnLSubState>(stateMachine));
	RegisterSubState(std::make_shared<NonCombatRunSubState::RunTurnRSubState>(stateMachine));
}
void PlayerNonCombatRunState::OnEnter()
{
	// フラグを立てる
	// TODO : ネットワークの処理修正
	//_owner->GetPlayer()->SetIsMoving(true);
	ChangeSubState("RunStart");

    // スタミナ自動回復を停止
    _owner->GetStaminaController()->SetIsStaminaRecover(false);
}

void PlayerNonCombatRunState::OnExecute(float elapsedTime)
{
	// スタミナ消費
	_owner->GetStaminaController()->ConsumeStamina(
		_owner->GetPlayer()->GetDashStaminaConsume() * elapsedTime,
		true);

	// スタミナが尽きたら遷移
	if (_owner->GetStaminaController()->GetStamina() <= 0.0f)
	{
		_owner->GetStateMachine().ChangeState("Fatigue");
		return;
    }

	// ダッシュ入力がなければ歩きへ遷移
	if (!_owner->GetPlayer()->IsDash())
	{
		_owner->GetStateMachine().ChangeState("Walk");
		_owner->GetStateMachine().ChangeSubState("Walking");
	}

	// 回避移行
	if (_owner->GetPlayer()->IsEvade())
		_owner->GetStateMachine().ChangeState("Evade");
	// ガード移行
	else if (_owner->GetPlayer()->IsGuard())
		_owner->GetStateMachine().ChangeState("CombatGuard");
	// 攻撃移行
	else if (_owner->GetPlayer()->IsAttack())
	{
		// 移動していたら攻撃へ
		if (_owner->GetPlayer()->IsMoving())
			_owner->GetStateMachine().ChangeState("CombatAttack1");
		// 移動していなければ抜刀
		else
			_owner->GetStateMachine().ChangeState("ToCombat");
	}
	// アイテム使用
	else if (_owner->GetPlayer()->IsUsingItem())
		_owner->ChangeItemState();
}
void PlayerNonCombatRunState::OnExit()
{
	// フラグをおろす
	// TODO : ネットワークの処理修正
	_owner->GetPlayer()->SetIsMoving(false);
	// スタミナ自動回復を再開
	_owner->GetStaminaController()->SetIsStaminaRecover(true);
}
#pragma endregion

#pragma region 回避
PlayerNonCombatEvadeState::PlayerNonCombatEvadeState(PlayerStateMachine* stateMachine) :
	Player8WayHSB(
		stateMachine,
		{ u8"RollF0",
		u8"RollFR45",
		u8"RollR90",
		u8"RollBR45",
		u8"RollB180",
		u8"RollBL45",
		u8"RollL90",
		u8"RollFL45", },
		0.2f,
		true)
{
}
void PlayerNonCombatEvadeState::OnEnter()
{
	// 入力方向から回避方向を決定
	Player8WayHSB::Direction directionType = Player8WayHSB::Direction::Front;
	Vector2 movement = _owner->GetPlayer()->GetMovement();

	// 入力方向が0なら前転
	if (movement.LengthSq() == 0.0f)
	{
		directionType = Player8WayHSB::Direction::Front;
	}
	else
	{
		// 入力方向とプレイヤーのY軸回転量から回避方向を決定
		float angle =
			DirectX::XMConvertToDegrees(
				atan2f(movement.x, movement.y)
				- _owner->GetPlayer()->GetActor()->GetTransform().GetAngle().y
			);
		// 角度を0~360度に正規化
		angle = fmodf(angle, 360.0f);
		if (angle < 0.0f)
			angle += 360.0f;
		// 360度を8方向に分割
		int index = (int)(angle / 45.0f + 0.5f);
		if (index >= 8)
			index = 0;
		directionType = static_cast<Player8WayHSB::Direction>(index);
	}
	ChangeSubState(directionType);

	// スタミナ自動回復を停止
	_owner->GetStaminaController()->SetIsStaminaRecover(false);
	// スタミナ消費
	_owner->GetStaminaController()->ConsumeStamina(
		_owner->GetPlayer()->GetEvadeStaminaConsume(),
		true);
}

void PlayerNonCombatEvadeState::OnExecute(float elapsedTime)
{
	// アニメーションが終了していたら遷移
	if (!_owner->GetAnimator()->IsPlaying())
		_owner->GetStateMachine().ChangeState("Idle");
	else if (_owner->GetPlayer()->CallCancelEvent())
	{
		// スタミナが尽きたら遷移
		if (_owner->GetStaminaController()->GetStamina() <= 0.0f)
		{
			_owner->GetStateMachine().ChangeState("Fatigue");
			return;
		}

		// 攻撃移行
		if (_owner->GetPlayer()->IsAttack())
			_owner->GetStateMachine().ChangeState("CombatAttack1");
		// 移動移行
		else if (_owner->GetPlayer()->IsMoving())
			_owner->GetStateMachine().ChangeState("Walk");
		// ガード移行
		else if (_owner->GetPlayer()->IsGuard())
			_owner->GetStateMachine().ChangeState("CombatGuard");
	}
}
void PlayerNonCombatEvadeState::OnExit()
{
	// スタミナ自動回復を再開
	_owner->GetStaminaController()->SetIsStaminaRecover(true);
}
#pragma endregion

#pragma region 抜刀
void PlayerNonCombatToCombatState::OnExecute(float elapsedTime)
{
	if (_owner->GetAnimator()->GetAnimationTimer() > _waitAttackTime)
	{
		// 攻撃をもう一度押しているなら攻撃へ遷移
		if (_owner->GetPlayer()->IsAttack() || _owner->GetPlayer()->IsHoldingAttackKey())
			_owner->GetStateMachine().ChangeState("CombatAttack1");

	}
	// アニメーションが終了していたら遷移
	if (!_owner->GetAnimator()->IsPlaying())
		_owner->GetStateMachine().ChangeState("CombatIdle");
}
#pragma endregion

#pragma region 被弾
PlayerNonCombatHitState::PlayerNonCombatHitState(PlayerStateMachine* stateMachine) :
	Player8WayHSB(
		stateMachine,
		{ u8"HitF",
		u8"HitF",
		u8"HitR",
		u8"HitB",
		u8"HitB",
		u8"HitB",
		u8"HitL",
		u8"HitF", },
		0.2f,
		true)
{
}

void PlayerNonCombatHitState::OnEnter()
{
	// プレイヤーの向きと、被弾方向から被弾アニメーションを決定
	auto& position = _owner->GetPlayer()->GetActor()->GetTransform().GetPosition();
	auto& hitPosition = _owner->GetPlayer()->GetDamageable()->GetHitPosition();
	auto hitDirection = (hitPosition - position).Normalize();

	float angle =
		DirectX::XMConvertToDegrees(
			atan2f(hitDirection.x, hitDirection.z)
			- _owner->GetPlayer()->GetActor()->GetTransform().GetAngle().y
		);
	// 角度を0~360度に正規化
	angle = fmodf(angle, 360.0f);
	if (angle < 0.0f)
		angle += 360.0f;
	// 360度を8方向に分割
	int index = (int)(angle / 45.0f + 0.5f);
	if (index >= 8)
		index = 0;
	Player8WayHSB::Direction directionType = static_cast<Player8WayHSB::Direction>(index);
	ChangeSubState(directionType);

	// 被弾モーション中は押し出されないようにする
	auto charactorController = _owner->GetPlayer()->GetCharactorController();
	if (charactorController != nullptr)
	{
		charactorController->SetIsPushable(false);
	}
}

void PlayerNonCombatHitState::OnExecute(float elapsedTime)
{
	// アニメーションが終了していたら遷移
	if (!_owner->GetAnimator()->IsPlaying())
		_owner->GetStateMachine().ChangeState("Idle");
}

void PlayerNonCombatHitState::OnExit()
{
	// 押し出されれるようにする
	auto charactorController = _owner->GetPlayer()->GetCharactorController();
	if (charactorController != nullptr)
	{
		charactorController->SetIsPushable(true);
	}
}

PlayerNonCombatHitKnockDownState::PlayerNonCombatHitKnockDownState(PlayerStateMachine* stateMachine) :
	Player8WayHSB(
		stateMachine,
		{ u8"HitLargeF",
		u8"HitLargeF",
		u8"HitLargeR",
		u8"HitLargeB",
		u8"HitLargeB",
		u8"HitLargeB",
		u8"HitLargeL",
		u8"HitLargeF", },
		0.2f,
		true)
{
}

void PlayerNonCombatHitKnockDownState::OnEnter()
{
	// プレイヤーの向きと、被弾方向から被弾アニメーションを決定
	auto& position = _owner->GetPlayer()->GetActor()->GetTransform().GetPosition();
	auto& hitPosition = _owner->GetPlayer()->GetDamageable()->GetHitPosition();
	auto hitDirection = (hitPosition - position).Normalize();

	float angle =
		DirectX::XMConvertToDegrees(
			atan2f(hitDirection.x, hitDirection.z)
			- _owner->GetPlayer()->GetActor()->GetTransform().GetAngle().y
		);
	// 角度を0~360度に正規化
	angle = fmodf(angle, 360.0f);
	if (angle < 0.0f)
		angle += 360.0f;
	// 360度を8方向に分割
	int index = (int)(angle / 45.0f + 0.5f);
	if (index >= 8)
		index = 0;
	Player8WayHSB::Direction directionType = static_cast<Player8WayHSB::Direction>(index);
	ChangeSubState(directionType);

	// 被弾モーション中は押し出されないようにする
	auto charactorController = _owner->GetPlayer()->GetCharactorController();
	if (charactorController != nullptr)
	{
		charactorController->SetIsPushable(false);
	}
}

void PlayerNonCombatHitKnockDownState::OnExecute(float elapsedTime)
{
	// アニメーションが終了していたら遷移
	if (!_owner->GetAnimator()->IsPlaying())
		_owner->GetStateMachine().ChangeState("Idle");
}

void PlayerNonCombatHitKnockDownState::OnExit()
{
	// 押し出されれるようにする
	auto charactorController = _owner->GetPlayer()->GetCharactorController();
	if (charactorController != nullptr)
	{
		charactorController->SetIsPushable(true);
	}
}
#pragma endregion

#pragma region ダウン
namespace PlayerNonCombatDownSubState
{
	class DownStartSubState : public PlayerSSB
	{
	public:
		DownStartSubState(PlayerStateMachine* stateMachine) :
			PlayerSSB(stateMachine,
				"DownStart",
				u8"HitLargeDeath",
				1.2f,
				false,
				true)
		{
		}
		~DownStartSubState() override {}
		void OnExecute(float elapsedTime) override
		{
			// アニメーションが終了していて、移動入力があれば遷移
			if (!_owner->GetAnimator()->IsPlaying())
			{
				if (_owner->GetPlayer()->IsMoving())
				{
					_owner->GetStateMachine().ChangeSubState("DownEnd");
					return;
				}
			}
		}
	};
	class DownEndSubState : public PlayerSSB
	{
	public:
		DownEndSubState(PlayerStateMachine* stateMachine) :
			PlayerSSB(stateMachine,
				"DownEnd",
				u8"GetUp",
				1.2f,
				false,
				true)
		{
		}
		~DownEndSubState() override {}
		void OnExecute(float elapsedTime) override
		{
			// アニメーションが終了していたら遷移
			if (!_owner->GetAnimator()->IsPlaying())
				_owner->GetStateMachine().ChangeState("Idle");
			// キャンセルイベントが呼ばれたら
			if (_owner->GetPlayer()->CallCancelEvent())
			{
				// 移動移行
				if (_owner->GetPlayer()->IsMoving())
					_owner->GetStateMachine().ChangeState("Walk");
				// ガード移行
				else if (_owner->GetPlayer()->IsGuard())
					_owner->GetStateMachine().ChangeState("CombatGuard");
			}
		}
	};
}
PlayerNonCombatDownState::PlayerNonCombatDownState(PlayerStateMachine* stateMachine) :
	HierarchicalStateBase(stateMachine)
{
	RegisterSubState(std::make_shared<PlayerNonCombatDownSubState::DownStartSubState>(stateMachine));
	RegisterSubState(std::make_shared<PlayerNonCombatDownSubState::DownEndSubState>(stateMachine));
}
void PlayerNonCombatDownState::OnEnter()
{
	// ダメージを受けた方向を向く
	Vector3 hitPosition = _owner->GetPlayer()->GetDamageable()->GetHitPosition();
	if (auto damageSender = _owner->GetPlayer()->GetDamageable()->GetLastDamageActor())
		hitPosition = damageSender->GetTransform().GetPosition();
	_owner->GetPlayer()->GetActor()->GetTransform().LookAt(hitPosition);
	// XZ回転量をリセット
	_owner->GetPlayer()->GetActor()->GetTransform().SetAngleX(0.0f);
	_owner->GetPlayer()->GetActor()->GetTransform().SetAngleZ(0.0f);

	ChangeSubState("DownStart");
	// モーション中は押し出されないようにする
	auto charactorController = _owner->GetPlayer()->GetCharactorController();
	if (charactorController != nullptr)
	{
		charactorController->SetIsPushable(false);
	}
}
void PlayerNonCombatDownState::OnExit()
{
	// 押し出されれるようにする
	auto charactorController = _owner->GetPlayer()->GetCharactorController();
	if (charactorController != nullptr)
	{
		charactorController->SetIsPushable(true);
	}
}
#pragma endregion

#pragma region 飲む
namespace PlayerDrinkSubState
{
	class DrinkStartSubState : public StateBase<PlayerStateMachine>
	{
	public:
		DrinkStartSubState(PlayerStateMachine* stateMachine) :
			StateBase(stateMachine)
		{
		}
		~DrinkStartSubState() override {}
		const char* GetName() const override { return "DrinkStart"; }

		void OnEnter() override
		{
			// 部分アニメーションを再生
			_owner->GetAnimator()->PlayPartialAnimation(u8"DrinkStart", false);
		}
		void OnExecute(float elapsedTime) override
		{
			// 部分アニメーションが終了していたら遷移
			if (_owner->GetAnimator()->IsPartialAnimationPlaying() == false)
				_owner->GetStateMachine().ChangeSubState("Drinking");
		}
		void OnExit() override {}
	};
	class DrinkingSubState : public StateBase<PlayerStateMachine>
	{
	public:
		DrinkingSubState(PlayerStateMachine* stateMachine) :
			StateBase(stateMachine)
		{
		}
		~DrinkingSubState() override {}

		const char* GetName() const override { return "Drinking"; }
		void OnEnter() override
		{
			// 部分アニメーションを再生
			_owner->GetAnimator()->PlayPartialAnimation(u8"Drinking", false, 0.0f);
			// 部分アニメーション用パラメータ設定
			_owner->GetAnimator()->RemovePartialAnimationMask("clavicle_r");
		}
		void OnExecute(float elapsedTime) override
		{
			// アイテム効果処理
			auto type = _owner->GetItemController()->ExecuteItemFunction(elapsedTime);

			// 部分アニメーションが終了したタイミングで定時間経過していたら遷移
			if (!_owner->GetAnimator()->IsPartialAnimationPlaying())
			{
				if (type == ItemFunctionBase::State::End)
				{
					_owner->GetStateMachine().ChangeSubState("DrinkEnd");
				}
				else
				{
					// 部分アニメーションを再生
					_owner->GetAnimator()->PlayPartialAnimation(u8"Drinking", false, 0.0f);
				}
			}
		}
		void OnExit() override
		{
			// 部分アニメーション用パラメータ設定
			_owner->GetAnimator()->SetPartialAnimationMask("clavicle_r");
		}
	};
	class DrinkEndSubState : public StateBase<PlayerStateMachine>
	{
	public:
		DrinkEndSubState(PlayerStateMachine* stateMachine) :
			StateBase(stateMachine)
		{
		}
		~DrinkEndSubState() override {}

		const char* GetName() const override { return "DrinkEnd"; }
		void OnEnter() override
		{
			// 部分アニメーションを再生
			_owner->GetAnimator()->PlayPartialAnimation(u8"DrinkEnd", false, 0.0f);
		}
		void OnExecute(float elapsedTime) override
		{
			// 部分アニメーションが終了していたら遷移
			if (!_owner->GetAnimator()->IsPartialAnimationPlaying())
			{
				if (_owner->GetPlayer()->IsMoving())
				{
					_owner->GetStateMachine().ChangeState("Walk");
                    _owner->GetStateMachine().ChangeSubState("Walking");
				}
                else
                    _owner->GetStateMachine().ChangeState("Idle");
			}
		}
		void OnExit() override {}
	};
}

PlayerNonCombatDrinkState::PlayerNonCombatDrinkState(PlayerStateMachine* stateMachine) :
	HierarchicalStateBase(stateMachine)
{
	// サブステート登録
	RegisterSubState(std::make_shared<PlayerDrinkSubState::DrinkStartSubState>(stateMachine));
	RegisterSubState(std::make_shared<PlayerDrinkSubState::DrinkingSubState>(stateMachine));
	RegisterSubState(std::make_shared<PlayerDrinkSubState::DrinkEndSubState>(stateMachine));
}

void PlayerNonCombatDrinkState::OnEnter()
{
	_owner->GetAnimator()->SetIsUseRootMotion(true);
	_owner->GetPlayer()->SetIsAbleToUseItem(false);
	ChangeSubState("DrinkStart");

	_owner->GetAnimator()->PlayAnimation(u8"Idle", true, 0.2f);
}

void PlayerNonCombatDrinkState::OnExecute(float elapsedTime)
{
	std::string stateName = _owner->GetStateName();
	if (stateName != "Drink")
		return;

	// 移動処理
	if (_owner->GetPlayer()->IsMoving())
	{
		if (_owner->GetAnimator()->GetAnimationName() != "RunLoopF0")
		{
			_owner->GetAnimator()->PlayAnimation(u8"RunLoopF0", true, 0.2f);
		}
		// 移動方向に向く
		_owner->RotationMovement(elapsedTime);
	}
	else
	{
		if (_owner->GetAnimator()->GetAnimationName() != "Idle")
			_owner->GetAnimator()->PlayAnimation(u8"Idle", true, 0.2f);
	}

	// 回避移行
	if (_owner->GetPlayer()->IsEvade())
		_owner->GetStateMachine().ChangeState("Evade");
}

void PlayerNonCombatDrinkState::OnExit()
{
	_owner->GetAnimator()->StopPartialAnimation(1.0f);
	_owner->GetPlayer()->SetIsAbleToUseItem(true);
}
#pragma endregion

#pragma region 疲労
namespace PlayerFatigueSubState
{
	class FatigueStartSubState : public PlayerSSB
	{
	public:
		FatigueStartSubState(PlayerStateMachine* stateMachine) :
			PlayerSSB(
				stateMachine,
				"FatigueStart",
				u8"KnockDownStart",
				1.0f,
				false,
				false)
		{
		}
		~FatigueStartSubState() override {}

		void OnExecute(float elapsedTime) override
		{
			// アニメーションが終了していたら遷移
			if (!_owner->GetAnimator()->IsPlaying())
				_owner->GetStateMachine().ChangeSubState("FatigueLoop");
		}
	};
	class FatigueLoopSubState : public PlayerSSB
	{
	public:
		FatigueLoopSubState(PlayerStateMachine* stateMachine) :
			PlayerSSB(
				stateMachine,
				"FatigueLoop",
				u8"KnockDownLoop",
				0.0f,
				true,
				false)
		{
		}
		~FatigueLoopSubState() override {}

		void OnExecute(float elapsedTime) override
		{
			// スタミナが回復したら終了に遷移
			if (_owner->GetStaminaController()->GetStamina() >=
				_owner->GetStaminaController()->GetMaxStamina())
			{
				_owner->GetStateMachine().ChangeSubState("FatigueEnd");
				return;
			}
		}
	};
	class FatigueEndSubState : public PlayerSSB
	{
	public:
		FatigueEndSubState(PlayerStateMachine* stateMachine) :
			PlayerSSB(
				stateMachine,
				"FatigueEnd",
				u8"KnockDownEnd",
				0.5f,
				false,
				false)
		{
		}
		~FatigueEndSubState() override {}

		void OnExecute(float elapsedTime) override
		{
			// アニメーションが終了していたら遷移
			if (!_owner->GetAnimator()->IsPlaying())
				_owner->GetStateMachine().ChangeState("Idle");
		}
	};
}
PlayerNonCombatFatigueState::PlayerNonCombatFatigueState(PlayerStateMachine* stateMachine) :
	HierarchicalStateBase(stateMachine)
{
	RegisterSubState(std::make_shared<PlayerFatigueSubState::FatigueStartSubState>(stateMachine));
	RegisterSubState(std::make_shared<PlayerFatigueSubState::FatigueLoopSubState>(stateMachine));
	RegisterSubState(std::make_shared<PlayerFatigueSubState::FatigueEndSubState>(stateMachine));
}

void PlayerNonCombatFatigueState::OnEnter()
{
	ChangeSubState("FatigueStart");
	// スタミナ自動回復を開始
	_owner->GetStaminaController()->SetIsStaminaRecover(true);
}

void PlayerNonCombatFatigueState::OnExecute(float elapsedTime)
{
	// スタミナを回復
	_owner->GetStaminaController()->RecoverStamina(_recoverStaminaSpeed * elapsedTime);
}

void PlayerNonCombatFatigueState::OnExit()
{
}
#pragma endregion
