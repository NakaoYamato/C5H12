#include "PlayerStateMachine.h"

#include "../../Library/Component/Animator.h"
#include "../PlayerController.h"
#include "../../Library/Math/Random.h"

#include <imgui.h>

PlayerStateMachine::PlayerStateMachine(PlayerController* player, Animator* animator) :
	_player(player),
    _animator(animator)
{
    // ステート設定
    _stateMachine.RegisterState(std::make_shared<PlayerIdleState>(this));
    _stateMachine.RegisterState(std::make_shared<PlayerRunState>(this));
    _stateMachine.RegisterState(std::make_shared<PlayerSprintState>(this));
    _stateMachine.RegisterState(std::make_shared<PlayerEvadeState>(this));
    _stateMachine.RegisterState(std::make_shared<PlayerAttack1State>(this));
	_stateMachine.RegisterState(std::make_shared<PlayerGuardState>(this));
	_stateMachine.RegisterState(std::make_shared<PlayerHitState>(this));
	_stateMachine.RegisterState(std::make_shared<PlayerHitKnockDownState>(this));
	_stateMachine.RegisterState(std::make_shared<PlayerGuardHitState>(this));
	_stateMachine.RegisterState(std::make_shared<PlayerGuardBreakState>(this));
	_stateMachine.RegisterState(std::make_shared<PlayerDeathState>(this));
}

// 開始処理
void PlayerStateMachine::Start()
{
    // 初期ステート設定
    _stateMachine.ChangeState("Idle");
}

// 実行処理
void PlayerStateMachine::Execute(float elapsedTime)
{

	if (_player->CallInvisivleEvent() && !_player->OldInvisibleEvent())
	{
		// 無敵状態に入る
		_player->GetDamageable()->SetInvisible(true);
	}
	else if (!_player->CallInvisivleEvent() && _player->OldInvisibleEvent())
	{
		// 無敵状態から抜ける
        _player->GetDamageable()->SetInvisible(false);
	}

    // 死亡処理
	if (_player->IsDead() && _stateMachine.GetStateName() != "Death")
	{
		_stateMachine.ChangeState("Death");
	}

    _stateMachine.Update(elapsedTime);
}

// Gui描画
void PlayerStateMachine::DrawGui()
{
    if (ImGui::Button(u8"死亡"))
    {
        _player->SetIsDead(true);
    }

    if (ImGui::TreeNode(u8"ステートマシン"))
    {
        std::string str = u8"現在のステート:";
        str += _stateMachine.GetState()->GetName();
        ImGui::Text("%s", str.c_str());
        str = u8"現在のサブステート:";
        str += _stateMachine.GetState()->GetSubStateName();
        ImGui::Text("%s", str.c_str());

        ImGui::Separator();
        for (auto& [name, state] : _stateMachine.GetStateMap())
        {
            if (ImGui::TreeNode(name.c_str()))
            {
                state->DrawGui();
                ImGui::TreePop();
            }
        }

        ImGui::TreePop();
    }
}

void PlayerStateMachine::RotationMovement(float elapsedTime, float rotationSpeed)
{
    auto charactorController = _player->GetActor()->GetComponent<CharactorController>();
    if (charactorController == nullptr)
        return;
    charactorController->UpdateRotation(elapsedTime, _player->GetMovement() * rotationSpeed);
}

// ステート変更
void PlayerStateMachine::ChangeState(const char* mainStateName, const char* subStateName)
{
    // 遷移先が無効な場合は何もしない
    if (mainStateName == nullptr || mainStateName[0] == '\0')
        return;

    std::string currentMainState = GetStateName();
    std::string currentSubState = GetSubStateName();

    // 現在のステートと変更先が違うなら変更
    if (currentMainState != mainStateName)
        _stateMachine.ChangeState(mainStateName);
    // サブステートがあるなら変更
    if (subStateName != nullptr && subStateName[0] != '\0')
    {
        // 現在のサブステートと変更先が違うなら変更
        if (currentSubState != subStateName)
            _stateMachine.ChangeSubState(subStateName);
    }
}

// ステート名取得
const char* PlayerStateMachine::GetStateName()
{
	if (!_stateMachine.GetState())
        return nullptr;
    return _stateMachine.GetState()->GetName();
}

const char* PlayerStateMachine::GetSubStateName()
{
    if (!_stateMachine.GetState())
        return nullptr;
    return _stateMachine.GetState()->GetSubStateName();
}

#pragma region 各ステート
#pragma region 待機
void PlayerIdleState::OnEnter()
{
    //_owner->GetAnimator()->PlayAnimation(u8"Idle1", true, 0.2f);
    _owner->GetAnimator()->PlayAnimation(u8"IdleCombat", true, 0.2f);
    _owner->GetAnimator()->SetIsUseRootMotion(false);
}

void PlayerIdleState::OnExecute(float elapsedTime)
{
    if (_owner->GetPlayer()->IsAttack())
        _owner->GetStateMachine().ChangeState("Attack1");
	else if (_owner->GetPlayer()->IsMoving())
        _owner->GetStateMachine().ChangeState("Run");
    // 回避移行
    else if (_owner->GetPlayer()->IsEvade())
        _owner->GetStateMachine().ChangeState("Evade");
	// ガード移行
	else if (_owner->GetPlayer()->IsGuard())
		_owner->GetStateMachine().ChangeState("Guard");
}
#pragma endregion

#pragma region 走り
void PlayerRunState::OnEnter()
{
    // フラグを立てる
    _owner->GetPlayer()->SetIsMoving(true);

    _owner->GetAnimator()->SetRootNodeIndex("root");
    _owner->GetAnimator()->SetIsUseRootMotion(true);
    //_owner->GetAnimator()->SetRootMotionOption(Animator::RootMotionOption::RemovePositionXY);
    _owner->GetAnimator()->SetRootMotionOption(Animator::RootMotionOption::None);
    //_owner->GetAnimator()->PlayAnimation(u8"Run_Forward_Rootmotion", true, 0.2f);
    _owner->GetAnimator()->PlayAnimation(u8"WalkCombatLoopF0", true, 0.2f);
}
void PlayerRunState::OnExecute(float elapsedTime)
{
    // 移動方向に向く
    _owner->RotationMovement(elapsedTime);

    if (_owner->GetPlayer()->IsAttack())
        _owner->GetStateMachine().ChangeState("Attack1");
    else if (_owner->GetPlayer()->IsDash())
        _owner->GetStateMachine().ChangeState("Sprint");
    else if (!_owner->GetPlayer()->IsMoving())
        _owner->GetStateMachine().ChangeState("Idle");
    // 回避移行
	else if (_owner->GetPlayer()->IsEvade())
		_owner->GetStateMachine().ChangeState("Evade");
    // ガード移行
    else if (_owner->GetPlayer()->IsGuard())
        _owner->GetStateMachine().ChangeState("Guard");
}
void PlayerRunState::OnExit()
{
    // フラグを下ろす
    _owner->GetPlayer()->SetIsMoving(false);
}
#pragma endregion

#pragma region ダッシュ
// ダッシュサブステート
namespace SprintSubState
{
    class SprintStartSubState : public StateBase<PlayerStateMachine>
    {
    public:
        SprintStartSubState(PlayerStateMachine* stateMachine) : StateBase(stateMachine)
        {
        }
        const char* GetName() const override { return "SprintStart"; }

        void OnEnter() override
        {
            _owner->GetAnimator()->SetRootNodeIndex("root");
            _owner->GetAnimator()->SetIsUseRootMotion(true);
            _owner->GetAnimator()->SetRootMotionOption(Animator::RootMotionOption::None);
            _owner->GetAnimator()->PlayAnimation(u8"RunCombatStartF0", false, 0.2f);
        }
        void OnExecute(float elapsedTime) override
        {
            // 攻撃移行
            if (_owner->GetPlayer()->IsAttack())
                _owner->GetStateMachine().ChangeSubState("SprintAttack");
            // 回避移行
            else if (_owner->GetPlayer()->IsEvade())
                _owner->GetStateMachine().ChangeState("Evade");
            // ガード移行
            else if (_owner->GetPlayer()->IsGuard())
                _owner->GetStateMachine().ChangeState("Guard");
            // アニメーションが終了していたら遷移
            else if (!_owner->GetAnimator()->IsPlayAnimation())
            {
                _owner->GetStateMachine().ChangeSubState("Sprinting");
            }
            // ダッシュ解除で移動に遷移
            else if (!_owner->GetPlayer()->IsDash())
            {
                _owner->GetStateMachine().ChangeState("Run");
            }
        }
        void OnExit() override
        {
        }
    };
    class SprintingSubState : public StateBase<PlayerStateMachine>
    {
    public:
        SprintingSubState(PlayerStateMachine* stateMachine) : StateBase(stateMachine)
        {
        }
        const char* GetName() const override { return "Sprinting"; }

        void OnEnter() override
        {
            _owner->GetAnimator()->SetRootNodeIndex("root");
            _owner->GetAnimator()->SetIsUseRootMotion(true);
            _owner->GetAnimator()->SetRootMotionOption(Animator::RootMotionOption::None);
            _owner->GetAnimator()->PlayAnimation(u8"RunCombatLoopF0", true, 0.2f);
        }
        void OnExecute(float elapsedTime) override
        {
			// 攻撃移行
            if (_owner->GetPlayer()->IsAttack())
                _owner->GetStateMachine().ChangeSubState("SprintAttack");
            // 回避移行
            else if (_owner->GetPlayer()->IsEvade())
                _owner->GetStateMachine().ChangeState("Evade");
            // ガード移行
            else if (_owner->GetPlayer()->IsGuard())
                _owner->GetStateMachine().ChangeState("Guard");
            // ダッシュ解除で移動に遷移
            else if (!_owner->GetPlayer()->IsDash())
                _owner->GetStateMachine().ChangeState("Run");
        }
        void OnExit() override
        {
        }
    };
    class SprintAttackSubState : public StateBase<PlayerStateMachine>
    {
    public:
        static constexpr float ATK = 1.0f;
    public:
        SprintAttackSubState(PlayerStateMachine* stateMachine) : StateBase(stateMachine)
        {
        }
        const char* GetName() const override { return "SprintAttack"; }

        void OnEnter() override
        {
            _owner->GetAnimator()->SetRootNodeIndex("root");
            _owner->GetAnimator()->SetIsUseRootMotion(true);
            _owner->GetAnimator()->SetRootMotionOption(Animator::RootMotionOption::None);
            _owner->GetAnimator()->PlayAnimation(u8"RunAttack01", false, 0.2f);

			// 攻撃フラグを立てる
            _owner->GetPlayer()->SetBaseATK(ATK);
        }
        void OnExecute(float elapsedTime) override
        {
            // アニメーションが終了していたら遷移
            if (!_owner->GetAnimator()->IsPlayAnimation())
                _owner->GetStateMachine().ChangeState("Idle");
            else if (_owner->GetPlayer()->CallCancelEvent())
            {
                // キャンセル攻撃
                if (_owner->GetPlayer()->IsAttack())
                    _owner->GetStateMachine().ChangeState("Attack1");
                // 回避移行
                else if (_owner->GetPlayer()->IsEvade())
                    _owner->GetStateMachine().ChangeState("Evade");
                // ガード移行
                else if (_owner->GetPlayer()->IsGuard())
                    _owner->GetStateMachine().ChangeState("Guard");
            }
        }
        void OnExit() override
        {
        }
    };
}

PlayerSprintState::PlayerSprintState(PlayerStateMachine* stateMachine) : 
    HierarchicalStateBase(stateMachine)
{
    // サブステート登録
    RegisterSubState(std::make_shared<SprintSubState::SprintStartSubState>(stateMachine));
    RegisterSubState(std::make_shared<SprintSubState::SprintingSubState>(stateMachine));
    RegisterSubState(std::make_shared<SprintSubState::SprintAttackSubState>(stateMachine));
}

void PlayerSprintState::OnEnter()
{
    // フラグを立てる
    _owner->GetPlayer()->SetIsDash(true);

    // 初期サブステート設定
    ChangeSubState("SprintStart");
}
void PlayerSprintState::OnExecute(float elapsedTime)
{
    // 移動方向に向く
    _owner->RotationMovement(elapsedTime);
}
void PlayerSprintState::OnExit()
{
    // フラグを下ろす
    _owner->GetPlayer()->SetIsDash(false);
}
#pragma endregion

#pragma region 回避
void PlayerEvadeState::OnEnter()
{
    static const std::string evadeAnimationNames[] =
    {
  //      u8"Evade_Forward",
		//u8"Evade_Forward_Right",
		//u8"Evade_Right",
		//u8"Evade_Backward_Right",
		//u8"Evade_Backward",
		//u8"Evade_Backward_Left",
  //      u8"Evade_Left",
  //      u8"Evade_Forward_Left",
        u8"RollCombatF0",
		u8"RollCombatFR45",
		u8"RollCombatR90",
		u8"RollCombatBR45",
		u8"RollCombatB180",
		u8"RollCombatBL45",
        u8"RollCombatL90",
        u8"RollCombatFL45",
    };

    _owner->GetAnimator()->SetRootNodeIndex("root");
    _owner->GetAnimator()->SetIsUseRootMotion(true);
    _owner->GetAnimator()->SetRootMotionOption(Animator::RootMotionOption::None);
	// 入力方向から回避方向を決定
	std::string evadeAnimationName = evadeAnimationNames[0];
    Vector2 movement = _owner->GetPlayer()->GetMovement();

    // 入力方向が0なら前転
    if (movement.LengthSq() == 0.0f)
        evadeAnimationName = evadeAnimationNames[0];
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
        evadeAnimationName = evadeAnimationNames[index];
    }

	_owner->GetAnimator()->PlayAnimation(evadeAnimationName, false, 0.2f);
}
void PlayerEvadeState::OnExecute(float elapsedTime)
{
    // アニメーションが終了していたら遷移
    if (!_owner->GetAnimator()->IsPlayAnimation())
    {
        _owner->GetStateMachine().ChangeState("Idle");
    }
    else if (_owner->GetPlayer()->CallCancelEvent())
    {
		// 攻撃移行
		if (_owner->GetPlayer()->IsAttack())
			_owner->GetStateMachine().ChangeState("Attack1");
		// 移動移行
		else if (_owner->GetPlayer()->IsMoving())
			_owner->GetStateMachine().ChangeState("Run");
        // ガード移行
        else if (_owner->GetPlayer()->IsGuard())
            _owner->GetStateMachine().ChangeState("Guard");
    }
}
#pragma endregion

#pragma region 攻撃1
// 攻撃1サブステート
namespace Attack1SubState
{
    class Combo1SubState : public StateBase<PlayerStateMachine>
    {
    public:
        static constexpr float ATK = 1.0f;
    public:
        Combo1SubState(PlayerStateMachine* stateMachine) : StateBase(stateMachine)
        {
        }
        const char* GetName() const override { return "ComboAttack1"; }

        void OnEnter() override
        {
            _owner->GetAnimator()->PlayAnimation(u8"ComboAttack01_01", false, 0.2f);
			// 攻撃フラグを立てる
			_owner->GetPlayer()->SetBaseATK(ATK);
        }
        void OnExecute(float elapsedTime) override
        {
            // 攻撃キャンセル判定
            if (_owner->GetPlayer()->CallCancelEvent())
                if (_owner->GetPlayer()->IsAttack())
                    _owner->GetStateMachine().ChangeSubState("ComboAttack2");
        }
        void OnExit() override 
        {
        }
    };
    class Combo2SubState : public StateBase<PlayerStateMachine>
    {
    public:
        static constexpr float ATK = 1.0f;
    public:
        Combo2SubState(PlayerStateMachine* stateMachine) : StateBase(stateMachine)
        {
        }
        const char* GetName() const override { return "ComboAttack2"; }

        void OnEnter() override
        {
            _owner->GetAnimator()->PlayAnimation(u8"ComboAttack01_02", false, 0.3f);
			// 攻撃フラグを立てる
			_owner->GetPlayer()->SetBaseATK(ATK);
        }
        void OnExecute(float elapsedTime) override
        {
            // 攻撃キャンセル判定
            if (_owner->GetPlayer()->CallCancelEvent())
                if (_owner->GetPlayer()->IsAttack())
                    _owner->GetStateMachine().ChangeSubState("ComboAttack3");
        }
        void OnExit() override 
        {
        }
    };
    class Combo3SubState : public StateBase<PlayerStateMachine>
    {
    public:
        static constexpr float ATK = 1.0f;
    public:
        Combo3SubState(PlayerStateMachine* stateMachine) : StateBase(stateMachine)
        {
        }
        const char* GetName() const override { return "ComboAttack3"; }

        void OnEnter() override
        {
            _owner->GetAnimator()->PlayAnimation(u8"ComboAttack01_03", false, 0.3f);
			// 攻撃フラグを立てる
			_owner->GetPlayer()->SetBaseATK(ATK);
        }
        void OnExecute(float elapsedTime) override
        {
            // 攻撃キャンセル判定
            if (_owner->GetPlayer()->CallCancelEvent())
                if (_owner->GetPlayer()->IsAttack())
                    _owner->GetStateMachine().ChangeSubState("ComboAttack4");
        }
        void OnExit() override 
        {
        }
    };
    class Combo4SubState : public StateBase<PlayerStateMachine>
    {
    public:
        static constexpr float ATK = 1.0f;
    public:
        Combo4SubState(PlayerStateMachine* stateMachine) : StateBase(stateMachine)
        {
        }
        const char* GetName() const override { return "ComboAttack4"; }

        void OnEnter() override
        {
            _owner->GetAnimator()->PlayAnimation(u8"ComboAttack01_04", false, 0.3f);
			// 攻撃フラグを立てる
			_owner->GetPlayer()->SetBaseATK(ATK);
        }
        void OnExecute(float elapsedTime) override
        {
        }
        void OnExit() override 
        {
        }
    };
}

PlayerAttack1State::PlayerAttack1State(PlayerStateMachine* stateMachine) : 
    HierarchicalStateBase(stateMachine)
{
	// サブステート登録
    RegisterSubState(std::make_shared<Attack1SubState::Combo1SubState>(stateMachine));
    RegisterSubState(std::make_shared<Attack1SubState::Combo2SubState>(stateMachine));
    RegisterSubState(std::make_shared<Attack1SubState::Combo3SubState>(stateMachine));
    RegisterSubState(std::make_shared<Attack1SubState::Combo4SubState>(stateMachine));
}

void PlayerAttack1State::OnEnter()
{
    _owner->GetAnimator()->SetRootNodeIndex("root");
    _owner->GetAnimator()->SetIsUseRootMotion(true);
    _owner->GetAnimator()->SetRootMotionOption(Animator::RootMotionOption::None);
    // 初期サブステート設定
	ChangeSubState("ComboAttack1");
}

void PlayerAttack1State::OnExecute(float elapsedTime)
{
    // 攻撃キャンセル判定
    if (_owner->GetPlayer()->CallCancelEvent())
    {
        // 回避移行
        if (_owner->GetPlayer()->IsEvade())
            _owner->GetStateMachine().ChangeState("Evade");
        // ガード移行
        else if (_owner->GetPlayer()->IsGuard())
            _owner->GetStateMachine().ChangeState("Guard");
    }
    else
    {
        // キャンセルがかかるまでの間は移動方向に向く
        _owner->RotationMovement(elapsedTime);
    }

    // アニメーションが終了していたら遷移
    if (!_owner->GetAnimator()->IsPlayAnimation())
    {
        // 攻撃からIdleに遷移
        _owner->GetStateMachine().ChangeState("Idle");
    }
}
#pragma endregion

#pragma region ガード
// ガードサブステート
namespace GuardSubState
{
    class GuardStartSubState : public StateBase<PlayerStateMachine>
    {
	public:
		GuardStartSubState(PlayerStateMachine* stateMachine) : StateBase(stateMachine)
		{
		}
		const char* GetName() const override { return "GuardStart"; }
		void OnEnter() override
		{
			_owner->GetAnimator()->PlayAnimation(u8"BlockStart", false, 2.0f);
		}
		void OnExecute(float elapsedTime) override
		{
            // アニメーションが終了していたら遷移
            if (!_owner->GetAnimator()->IsPlayAnimation())
                _owner->GetStateMachine().ChangeSubState("Guarding");
		}
		void OnExit() override
		{
		}
    };

	class GuardingSubState : public StateBase<PlayerStateMachine>
	{
	public:
		GuardingSubState(PlayerStateMachine* stateMachine) : StateBase(stateMachine)
		{
		}
		const char* GetName() const override { return "Guarding"; }
		void OnEnter() override
		{
			_owner->GetAnimator()->PlayAnimation(u8"BlockLoop", true, 0.2f);
		}
		void OnExecute(float elapsedTime) override
		{
		}
		void OnExit() override
		{
		}
	};
}

PlayerGuardState::PlayerGuardState(PlayerStateMachine* stateMachine) :
    HierarchicalStateBase(stateMachine)
{
    // サブステート登録
	RegisterSubState(std::make_shared<GuardSubState::GuardStartSubState>(stateMachine));
	RegisterSubState(std::make_shared<GuardSubState::GuardingSubState>(stateMachine));
}

void PlayerGuardState::OnEnter()
{
    // フラグを立てる
    _owner->GetPlayer()->SetIsGuard(true);

    _owner->GetAnimator()->SetRootNodeIndex("root");
    _owner->GetAnimator()->SetIsUseRootMotion(true);
    _owner->GetAnimator()->SetRootMotionOption(Animator::RootMotionOption::None);
	// 初期サブステート設定
	ChangeSubState("GuardStart");
}

void PlayerGuardState::OnExecute(float elapsedTime)
{
    // 移動方向に向く
    _owner->RotationMovement(elapsedTime);

    // 攻撃移行
    if (_owner->GetPlayer()->IsAttack())
        _owner->GetStateMachine().ChangeState("Attack1");
    // 回避移行
    else if (_owner->GetPlayer()->IsEvade())
        _owner->GetStateMachine().ChangeState("Evade");
    // ガード解除
    else if (!_owner->GetPlayer()->IsGuard())
        _owner->GetStateMachine().ChangeState("Idle");
}

void PlayerGuardState::OnExit()
{
    // フラグを下ろす
    _owner->GetPlayer()->SetIsGuard(false);
}

#pragma endregion

#pragma region 被弾
void PlayerHitState::OnEnter()
{
	static const std::string HitAnimationNames[] =
	{
		u8"HitCombatF",
		//u8"Hit2",
		//u8"Hit3",
		//u8"Hit4",
		//u8"Hit5",
	};

    _owner->GetAnimator()->SetRootNodeIndex("root");
    _owner->GetAnimator()->SetIsUseRootMotion(true);
    _owner->GetAnimator()->SetRootMotionOption(Animator::RootMotionOption::None);
    _owner->GetAnimator()->PlayAnimation(HitAnimationNames[std::rand() % _countof(HitAnimationNames)], false, 0.2f);

    // 被弾モーション中は押し出されないようにする
    auto charactorController = _owner->GetPlayer()->GetCharactorController();
	if (charactorController != nullptr)
	{
		charactorController->SetIsPushable(false);
	}
}

void PlayerHitState::OnExecute(float elapsedTime)
{
    // アニメーションが終了していたら遷移
    if (!_owner->GetAnimator()->IsPlayAnimation())
        _owner->GetStateMachine().ChangeState("Idle");
}

void PlayerHitState::OnExit()
{
    // 押し出されれるようにする
    auto charactorController = _owner->GetPlayer()->GetCharactorController();
    if (charactorController != nullptr)
    {
        charactorController->SetIsPushable(true);
    }
}

void PlayerHitKnockDownState::OnEnter()
{
    _owner->GetAnimator()->SetRootNodeIndex("root");
    _owner->GetAnimator()->SetIsUseRootMotion(true);
    _owner->GetAnimator()->SetRootMotionOption(Animator::RootMotionOption::None);
    _owner->GetAnimator()->PlayAnimation("HitCombatLargeF", false, 0.2f);

    // 被弾モーション中は押し出されないようにする
    auto charactorController = _owner->GetPlayer()->GetCharactorController();
    if (charactorController != nullptr)
    {
        charactorController->SetIsPushable(false);
    }
}

void PlayerHitKnockDownState::OnExecute(float elapsedTime)
{
    // アニメーションが終了していたら遷移
    if (!_owner->GetAnimator()->IsPlayAnimation())
        _owner->GetStateMachine().ChangeState("Idle");
}

void PlayerHitKnockDownState::OnExit()
{
    // 押し出されれるようにする
    auto charactorController = _owner->GetPlayer()->GetCharactorController();
    if (charactorController != nullptr)
    {
        charactorController->SetIsPushable(true);
    }
}

void PlayerGuardHitState::OnEnter()
{
    static const std::string GuardHitAnimationNames[] =
    {
        u8"BlockHit",
        //u8"Guard_Hit2",
        //u8"Guard_Hit3",
    };

    _owner->GetAnimator()->SetRootNodeIndex("root");
    _owner->GetAnimator()->SetIsUseRootMotion(true);
    _owner->GetAnimator()->SetRootMotionOption(Animator::RootMotionOption::None);
    _owner->GetAnimator()->PlayAnimation(GuardHitAnimationNames[std::rand() % _countof(GuardHitAnimationNames)], false, 0.2f);

    // 被弾モーション中は押し出されないようにする
    auto charactorController = _owner->GetPlayer()->GetCharactorController();
    if (charactorController != nullptr)
    {
        charactorController->SetIsPushable(false);
    }
}

void PlayerGuardHitState::OnExecute(float elapsedTime)
{
    // アニメーションが終了していたら遷移
    if (!_owner->GetAnimator()->IsPlayAnimation())
        _owner->GetStateMachine().ChangeState("Idle");
}

void PlayerGuardHitState::OnExit()
{
    // 押し出されれるようにする
    auto charactorController = _owner->GetPlayer()->GetCharactorController();
    if (charactorController != nullptr)
    {
        charactorController->SetIsPushable(true);
    }
}

void PlayerGuardBreakState::OnEnter()
{
    _owner->GetAnimator()->SetRootNodeIndex("root");
    _owner->GetAnimator()->SetIsUseRootMotion(true);
    _owner->GetAnimator()->SetRootMotionOption(Animator::RootMotionOption::None);
    _owner->GetAnimator()->PlayAnimation("BlockHitBreak", false, 0.2f);

    // 被弾モーション中は押し出されないようにする
    auto charactorController = _owner->GetPlayer()->GetCharactorController();
    if (charactorController != nullptr)
    {
        charactorController->SetIsPushable(false);
    }
}

void PlayerGuardBreakState::OnExecute(float elapsedTime)
{
    // アニメーションが終了していたら遷移
    if (!_owner->GetAnimator()->IsPlayAnimation())
        _owner->GetStateMachine().ChangeState("Idle");
}

void PlayerGuardBreakState::OnExit()
{
    // 押し出されれるようにする
    auto charactorController = _owner->GetPlayer()->GetCharactorController();
    if (charactorController != nullptr)
    {
        charactorController->SetIsPushable(true);
    }
}

#pragma endregion

#pragma region 死亡
void PlayerDeathState::OnEnter()
{
    static const std::string DeathAnimationNames[] =
    {
        u8"HitCombatDeath",
        //u8"Death2",
        //u8"Hit_knockdown_Death",
    };

    _owner->GetAnimator()->SetRootNodeIndex("root");
    _owner->GetAnimator()->SetIsUseRootMotion(true);
    _owner->GetAnimator()->SetRootMotionOption(Animator::RootMotionOption::None);
    _owner->GetAnimator()->PlayAnimation(DeathAnimationNames[std::rand() % _countof(DeathAnimationNames)], false, 0.2f);
}

void PlayerDeathState::OnExecute(float elapsedTime)
{
}
#pragma endregion

#pragma endregion
