#include "PlayerNonCombatStates.h"

#include "../../Library/Component/Animator.h"
#include "../PlayerController.h"
#include "../../Library/Algorithm/Converter.h"

#pragma region 待機
void PlayerNonCombatIdleState::OnExecute(float elapsedTime)
{
	// 抜刀遷移
	if (_owner->GetPlayer()->IsAttack())
		_owner->GetStateMachine().ChangeState("ToCombat");
    // 移動
    else if (_owner->GetPlayer()->IsMoving())
        _owner->GetStateMachine().ChangeState("Walk");
    // 回避移行
    else if (_owner->GetPlayer()->IsEvade())
        _owner->GetStateMachine().ChangeState("Evade");
    // ガード移行
    else if (_owner->GetPlayer()->IsGuard())
        _owner->GetStateMachine().ChangeState("CombatGuard");
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
			// 移動方向に向く
			_owner->RotationMovement(elapsedTime);
			// アニメーションが終了していたら遷移
			if (!_owner->GetAnimator()->IsPlayAnimation())
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
			else if (!_owner->GetAnimator()->IsPlayAnimation())
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
	_owner->GetPlayer()->SetIsMoving(true);
	ChangeSubState("WalkStart");
}

void PlayerNonCombatWalkState::OnExecute(float elapsedTime)
{
	// 移動があって、ダッシュ入力があれば走りへ遷移
	if (_owner->GetPlayer()->IsMoving() && _owner->GetPlayer()->IsDash())
	{
		_owner->GetStateMachine().ChangeState("Run");
		_owner->GetStateMachine().ChangeSubState("Running");
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
}
void PlayerNonCombatWalkState::OnExit()
{
	// フラグをおろす
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
				u8"RunFastStart", 0.2f,
				false,
				true)
		{
		}
		~RunStartSubState() override {}
		void OnExecute(float elapsedTime) override
		{
			// 移動方向に向く
			_owner->RotationMovement(elapsedTime);
			// アニメーションが終了していたら遷移
			if (!_owner->GetAnimator()->IsPlayAnimation())
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
				u8"RunFastLoop", 0.2f,
				true,
				true)
		{
		}
		~RunningSubState() override {}
		void OnExecute(float elapsedTime) override
		{
			// 移動方向に向く
			_owner->RotationMovement(elapsedTime);
			// 移動していなければ終了に遷移
			if (!_owner->GetPlayer()->IsMoving())
				_owner->GetStateMachine().ChangeSubState("RunStop");
		}
	};
	// 走り停止
	class RunStopSubState : public PlayerSSB
	{
	public:
		RunStopSubState(PlayerStateMachine* stateMachine) :
			PlayerSSB(stateMachine,
				"RunStop",
				u8"RunFastStop", 0.2f,
				false,
				true)
		{
		}
		~RunStopSubState() override {}
		void OnExecute(float elapsedTime) override
		{
			// 移動方向に向く
			_owner->RotationMovement(elapsedTime);
			// 移動があれば遷移
			if (_owner->GetPlayer()->IsMoving())
				_owner->GetStateMachine().ChangeSubState("Running");
			// アニメーションが終了していたら遷移
			else if (!_owner->GetAnimator()->IsPlayAnimation())
				_owner->GetStateMachine().ChangeState("Idle");
		}
	};
}
PlayerNonCombatRunState::PlayerNonCombatRunState(PlayerStateMachine* stateMachine) : 
	HierarchicalStateBase(stateMachine)
{
	RegisterSubState(std::make_shared<NonCombatRunSubState::RunStartSubState>(stateMachine));
	RegisterSubState(std::make_shared<NonCombatRunSubState::RunningSubState>(stateMachine));
	RegisterSubState(std::make_shared<NonCombatRunSubState::RunStopSubState>(stateMachine));
}
void PlayerNonCombatRunState::OnEnter()
{
	// フラグを立てる
	_owner->GetPlayer()->SetIsMoving(true);
	ChangeSubState("RunStart");
}

void PlayerNonCombatRunState::OnExecute(float elapsedTime)
{
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
}
void PlayerNonCombatRunState::OnExit()
{
	// フラグをおろす
	_owner->GetPlayer()->SetIsMoving(false);
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
				- _owner->GetPlayer()->GetActor()->GetTransform().GetRotation().y
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
}

void PlayerNonCombatEvadeState::OnExecute(float elapsedTime)
{
	// アニメーションが終了していたら遷移
	if (!_owner->GetAnimator()->IsPlayAnimation())
		_owner->GetStateMachine().ChangeState("Idle");
	else if (_owner->GetPlayer()->CallCancelEvent())
	{
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
#pragma endregion

#pragma region 抜刀
void PlayerNonCombatToCombatState::OnExecute(float elapsedTime)
{
	// アニメーションが終了していたら遷移
	if (!_owner->GetAnimator()->IsPlayAnimation())
		_owner->GetStateMachine().ChangeState("CombatIdle");
}
#pragma endregion
