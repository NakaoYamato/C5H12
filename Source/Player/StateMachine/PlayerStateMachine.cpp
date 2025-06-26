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
    _stateMachine.ChangeState(Network::GetPlayerMainStateName(Network::PlayerMainStates::Idle));
}

// 実行処理
void PlayerStateMachine::Execute(float elapsedTime)
{
    _callCancelEvent = false;
	bool oldInvisibleEvent = _callInvisivleEvent;
	_callInvisivleEvent = false;

    // アニメーションイベント取得
    if (GetAnimator()->IsPlayAnimation())
    {
        auto& animationEvent = GetAnimator()->GetAnimationEvent();
        int massageListSize = (int)animationEvent.GetMessageList().size();
        auto events = GetAnimator()->GetCurrentEvents();
        for (auto& event : events)
        {
            // メッセージインデックスが範囲外ならcontinue
            if (event.messageIndex < 0 || event.messageIndex >= massageListSize)
                continue;

            // 攻撃キャンセル判定
            if (animationEvent.GetMessageList().at(event.messageIndex) == "Cancel")
            {
                _callCancelEvent = true;
            }
            // 無敵判定
            if (animationEvent.GetMessageList().at(event.messageIndex) == "Invisible")
            {
                _callInvisivleEvent = true;
            }
        }
    }

	if (_callInvisivleEvent && !oldInvisibleEvent)
	{
		// 無敵状態に入る
		_player->GetDamageable()->SetInvisible(true);
	}
	else if (!_callInvisivleEvent && oldInvisibleEvent)
	{
		// 無敵状態から抜ける
        _player->GetDamageable()->SetInvisible(false);
	}

    // 死亡処理
	if (IsDead() && _stateMachine.GetStateName() != Network::GetPlayerMainStateName(Network::PlayerMainStates::Death))
	{
		_stateMachine.ChangeState(Network::GetPlayerMainStateName(Network::PlayerMainStates::Death));
	}

    _stateMachine.Update(elapsedTime);
}

// Gui描画
void PlayerStateMachine::DrawGui()
{
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
    charactorController->UpdateRotation(elapsedTime, _movement * rotationSpeed);
}

// ステート変更
void PlayerStateMachine::ChangeState(Network::PlayerMainStates mainStateName, Network::PlayerSubStates subStateName)
{
    Network::PlayerMainStates currentMainState = Network::GetPlayerMainStateFromName(GetStateName());
    Network::PlayerSubStates currentSubState = Network::GetPlayerSubStateFromName(GetSubStateName());

    // 現在のステートと変更先が違うなら変更
    if (currentMainState != mainStateName)
        _stateMachine.ChangeState(Network::GetPlayerMainStateName(mainStateName));

    // サブステートがあるなら変更
    if (subStateName != Network::PlayerSubStates::None)
    {
        // 現在のサブステートと変更先が違うなら変更
        if (currentSubState != subStateName)
            _stateMachine.ChangeSubState(Network::GetPlayerSubStateName(subStateName));
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
const char* PlayerIdleState::GetName() const
{
    return Network::GetPlayerMainStateName(Network::PlayerMainStates::Idle);
}
void PlayerIdleState::OnEnter()
{
    owner->GetAnimator()->PlayAnimation(u8"Idle1", true, 0.2f);
    owner->GetAnimator()->SetIsUseRootMotion(false);
}

void PlayerIdleState::OnExecute(float elapsedTime)
{
    if (owner->IsAttack())
        owner->GetStateMachine().ChangeState(Network::GetPlayerMainStateName(Network::PlayerMainStates::Attack1));
	else if (owner->IsMoving())
        owner->GetStateMachine().ChangeState(Network::GetPlayerMainStateName(Network::PlayerMainStates::Run));
    // 回避移行
    else if (owner->IsEvade())
        owner->GetStateMachine().ChangeState(Network::GetPlayerMainStateName(Network::PlayerMainStates::Evade));
	// ガード移行
	else if (owner->IsGuard())
		owner->GetStateMachine().ChangeState(Network::GetPlayerMainStateName(Network::PlayerMainStates::Guard));
}
#pragma endregion

#pragma region 走り
const char* PlayerRunState::GetName() const
{
    return Network::GetPlayerMainStateName(Network::PlayerMainStates::Run);
}
void PlayerRunState::OnEnter()
{
    // フラグを立てる
    owner->SetIsMoving(true);

    owner->GetAnimator()->SetRootNodeIndex("root");
    owner->GetAnimator()->SetIsUseRootMotion(true);
    owner->GetAnimator()->SetRootMotionOption(Animator::RootMotionOption::RemovePositionXY);
    owner->GetAnimator()->PlayAnimation(u8"Run_Forward_Rootmotion", true, 0.2f);
}
void PlayerRunState::OnExecute(float elapsedTime)
{
    // 移動方向に向く
    owner->RotationMovement(elapsedTime);

    if (owner->IsAttack())
        owner->GetStateMachine().ChangeState(Network::GetPlayerMainStateName(Network::PlayerMainStates::Attack1));
    else if (owner->IsDash())
        owner->GetStateMachine().ChangeState(Network::GetPlayerMainStateName(Network::PlayerMainStates::Sprint));
    else if (!owner->IsMoving())
        owner->GetStateMachine().ChangeState(Network::GetPlayerMainStateName(Network::PlayerMainStates::Idle));
    // 回避移行
	else if (owner->IsEvade())
		owner->GetStateMachine().ChangeState(Network::GetPlayerMainStateName(Network::PlayerMainStates::Evade));
    // ガード移行
    else if (owner->IsGuard())
        owner->GetStateMachine().ChangeState(Network::GetPlayerMainStateName(Network::PlayerMainStates::Guard));
}
void PlayerRunState::OnExit()
{
    // フラグを下ろす
    owner->SetIsMoving(false);
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
        const char* GetName() const override { return Network::GetPlayerSubStateName(Network::PlayerSubStates::SprintStart); }

        void OnEnter() override
        {
            owner->GetAnimator()->PlayAnimation(u8"Sprint_Start_no_weapon_Rootmotion", false, 0.2f);
        }
        void OnExecute(float elapsedTime) override
        {
            // 攻撃移行
            if (owner->IsAttack())
                owner->GetStateMachine().ChangeSubState(Network::GetPlayerSubStateName(Network::PlayerSubStates::SprintAttack));
            // 回避移行
            else if (owner->IsEvade())
                owner->GetStateMachine().ChangeState(Network::GetPlayerMainStateName(Network::PlayerMainStates::Evade));
            // ガード移行
            else if (owner->IsGuard())
                owner->GetStateMachine().ChangeState(Network::GetPlayerMainStateName(Network::PlayerMainStates::Guard));
            // アニメーションが終了していたら遷移
            else if (!owner->GetAnimator()->IsPlayAnimation())
            {
                owner->GetStateMachine().ChangeSubState(Network::GetPlayerSubStateName(Network::PlayerSubStates::Sprinting));
            }
            // ダッシュ解除で移動に遷移
            else if (!owner->IsDash())
            {
                owner->GetStateMachine().ChangeState(Network::GetPlayerMainStateName(Network::PlayerMainStates::Run));
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
        const char* GetName() const override { return Network::GetPlayerSubStateName(Network::PlayerSubStates::Sprinting); }

        void OnEnter() override
        {
            owner->GetAnimator()->SetRootNodeIndex("root");
            owner->GetAnimator()->SetIsUseRootMotion(true);
            owner->GetAnimator()->SetRootMotionOption(Animator::RootMotionOption::RemovePositionXY);
            owner->GetAnimator()->PlayAnimation(u8"Sprint_no_weapon_Rootmotion", true, 0.2f);
        }
        void OnExecute(float elapsedTime) override
        {
			// 攻撃移行
            if (owner->IsAttack())
                owner->GetStateMachine().ChangeSubState(Network::GetPlayerSubStateName(Network::PlayerSubStates::SprintAttack));
            // 回避移行
            else if (owner->IsEvade())
                owner->GetStateMachine().ChangeState(Network::GetPlayerMainStateName(Network::PlayerMainStates::Evade));
            // ガード移行
            else if (owner->IsGuard())
                owner->GetStateMachine().ChangeState(Network::GetPlayerMainStateName(Network::PlayerMainStates::Guard));
            // ダッシュ解除で移動に遷移
            else if (!owner->IsDash())
                owner->GetStateMachine().ChangeState(Network::GetPlayerMainStateName(Network::PlayerMainStates::Run));
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
        const char* GetName() const override { return Network::GetPlayerSubStateName(Network::PlayerSubStates::SprintAttack); }

        void OnEnter() override
        {
            owner->GetAnimator()->SetRootNodeIndex("ORG-hips");
            owner->GetAnimator()->SetIsUseRootMotion(true);
            owner->GetAnimator()->SetRootMotionOption(Animator::RootMotionOption::RemovePositionXY);
            owner->GetAnimator()->PlayAnimation(u8"Attack_Sprint", false, 0.2f);

			// 攻撃フラグを立てる
            owner->GetPlayer()->SetBaseATK(ATK);
        }
        void OnExecute(float elapsedTime) override
        {
            // アニメーションが終了していたら遷移
            if (!owner->GetAnimator()->IsPlayAnimation())
                owner->GetStateMachine().ChangeState(Network::GetPlayerMainStateName(Network::PlayerMainStates::Idle));
            else if (owner->CallCancelEvent())
            {
                // キャンセル攻撃
                if (owner->IsAttack())
                    owner->GetStateMachine().ChangeState(Network::GetPlayerMainStateName(Network::PlayerMainStates::Attack1));
                // 回避移行
                else if (owner->IsEvade())
                    owner->GetStateMachine().ChangeState(Network::GetPlayerMainStateName(Network::PlayerMainStates::Evade));
                // ガード移行
                else if (owner->IsGuard())
                    owner->GetStateMachine().ChangeState(Network::GetPlayerMainStateName(Network::PlayerMainStates::Guard));
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

const char* PlayerSprintState::GetName() const
{
    return Network::GetPlayerMainStateName(Network::PlayerMainStates::Sprint);
}

void PlayerSprintState::OnEnter()
{
    // フラグを立てる
    owner->SetIsDash(true);

    owner->GetAnimator()->SetRootNodeIndex("root");
    owner->GetAnimator()->SetIsUseRootMotion(true);
    owner->GetAnimator()->SetRootMotionOption(Animator::RootMotionOption::RemovePositionXY);
    // 初期サブステート設定
    ChangeSubState(Network::GetPlayerSubStateName(Network::PlayerSubStates::SprintStart));
}
void PlayerSprintState::OnExecute(float elapsedTime)
{
    // 移動方向に向く
    owner->RotationMovement(elapsedTime);
}
void PlayerSprintState::OnExit()
{
    // フラグを下ろす
    owner->SetIsDash(false);
}
#pragma endregion

#pragma region 回避
const char* PlayerEvadeState::GetName() const
{
    return Network::GetPlayerMainStateName(Network::PlayerMainStates::Evade);
}
void PlayerEvadeState::OnEnter()
{
    static const std::string evadeAnimationNames[] =
    {
        u8"Evade_Forward",
		u8"Evade_Forward_Right",
		u8"Evade_Right",
		u8"Evade_Backward_Right",
		u8"Evade_Backward",
		u8"Evade_Backward_Left",
        u8"Evade_Left",
        u8"Evade_Forward_Left",
    };

    owner->GetAnimator()->SetRootNodeIndex("ORG-hips");
    owner->GetAnimator()->SetIsUseRootMotion(true);
	owner->GetAnimator()->SetRootMotionOption(Animator::RootMotionOption::RemovePositionXY);
	// 入力方向から回避方向を決定
	std::string evadeAnimationName = evadeAnimationNames[0];
    Vector2 movement = owner->GetMovement();

    // 入力方向が0なら前転
    if (movement.LengthSq() == 0.0f)
        evadeAnimationName = evadeAnimationNames[0];
    else
    {
        // 入力方向とプレイヤーのY軸回転量から回避方向を決定
        float angle =
            DirectX::XMConvertToDegrees(
                atan2f(movement.x, movement.y)
                - owner->GetPlayer()->GetActor()->GetTransform().GetRotation().y
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

	owner->GetAnimator()->PlayAnimation(evadeAnimationName, false, 0.2f);
}
void PlayerEvadeState::OnExecute(float elapsedTime)
{
    // アニメーションが終了していたら遷移
    if (!owner->GetAnimator()->IsPlayAnimation())
    {
        owner->GetStateMachine().ChangeState(Network::GetPlayerMainStateName(Network::PlayerMainStates::Idle));
    }
    else if (owner->CallCancelEvent())
    {
		// 攻撃移行
		if (owner->IsAttack())
			owner->GetStateMachine().ChangeState(Network::GetPlayerMainStateName(Network::PlayerMainStates::Attack1));
		// 移動移行
		else if (owner->IsMoving())
			owner->GetStateMachine().ChangeState(Network::GetPlayerMainStateName(Network::PlayerMainStates::Run));
        // ガード移行
        else if (owner->IsGuard())
            owner->GetStateMachine().ChangeState(Network::GetPlayerMainStateName(Network::PlayerMainStates::Guard));
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
        const char* GetName() const override { return Network::GetPlayerSubStateName(Network::PlayerSubStates::ComboAttack1); }

        void OnEnter() override
        {
            owner->GetAnimator()->PlayAnimation(u8"Attack_Combo1", false, 0.2f);
			// 攻撃フラグを立てる
			owner->GetPlayer()->SetBaseATK(ATK);
        }
        void OnExecute(float elapsedTime) override
        {
            // 攻撃キャンセル判定
            if (owner->CallCancelEvent())
                if (owner->IsAttack())
                    owner->GetStateMachine().ChangeSubState(Network::GetPlayerSubStateName(Network::PlayerSubStates::ComboAttack2));
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
        const char* GetName() const override { return Network::GetPlayerSubStateName(Network::PlayerSubStates::ComboAttack2); }

        void OnEnter() override
        {
            owner->GetAnimator()->PlayAnimation(u8"Attack_Combo2", false, 0.3f);
			// 攻撃フラグを立てる
			owner->GetPlayer()->SetBaseATK(ATK);
        }
        void OnExecute(float elapsedTime) override
        {
            // 攻撃キャンセル判定
            if (owner->CallCancelEvent())
                if (owner->IsAttack())
                    owner->GetStateMachine().ChangeSubState(Network::GetPlayerSubStateName(Network::PlayerSubStates::ComboAttack3));
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
        const char* GetName() const override { return Network::GetPlayerSubStateName(Network::PlayerSubStates::ComboAttack3); }

        void OnEnter() override
        {
            owner->GetAnimator()->PlayAnimation(u8"Attack_Combo3", false, 0.3f);
			// 攻撃フラグを立てる
			owner->GetPlayer()->SetBaseATK(ATK);
        }
        void OnExecute(float elapsedTime) override
        {
            // 攻撃キャンセル判定
            if (owner->CallCancelEvent())
                if (owner->IsAttack())
                    owner->GetStateMachine().ChangeSubState(Network::GetPlayerSubStateName(Network::PlayerSubStates::ComboAttack4));
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
        const char* GetName() const override { return Network::GetPlayerSubStateName(Network::PlayerSubStates::ComboAttack4); }

        void OnEnter() override
        {
            owner->GetAnimator()->PlayAnimation(u8"Attack_Combo4", false, 0.3f);
			// 攻撃フラグを立てる
			owner->GetPlayer()->SetBaseATK(ATK);
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

const char* PlayerAttack1State::GetName() const
{
    return Network::GetPlayerMainStateName(Network::PlayerMainStates::Attack1);
}

void PlayerAttack1State::OnEnter()
{
    owner->GetAnimator()->SetRootNodeIndex("ORG-hips");
    owner->GetAnimator()->SetIsUseRootMotion(true);
    owner->GetAnimator()->SetRootMotionOption(Animator::RootMotionOption::RemovePositionXY);
    // 初期サブステート設定
	ChangeSubState(Network::GetPlayerSubStateName(Network::PlayerSubStates::ComboAttack1));
}

void PlayerAttack1State::OnExecute(float elapsedTime)
{
    // 攻撃キャンセル判定
    if (owner->CallCancelEvent())
    {
        // 回避移行
        if (owner->IsEvade())
            owner->GetStateMachine().ChangeState(Network::GetPlayerMainStateName(Network::PlayerMainStates::Evade));
        // ガード移行
        else if (owner->IsGuard())
            owner->GetStateMachine().ChangeState(Network::GetPlayerMainStateName(Network::PlayerMainStates::Guard));
    }
    else
    {
        // キャンセルがかかるまでの間は移動方向に向く
        owner->RotationMovement(elapsedTime);
    }

    // アニメーションが終了していたら遷移
    if (!owner->GetAnimator()->IsPlayAnimation())
    {
        // 攻撃からIdleに遷移
        owner->GetStateMachine().ChangeState(Network::GetPlayerMainStateName(Network::PlayerMainStates::Idle));
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
		const char* GetName() const override { return Network::GetPlayerSubStateName(Network::PlayerSubStates::GuardStart); }
		void OnEnter() override
		{
			owner->GetAnimator()->PlayAnimation(u8"Guard_Start", false, 2.0f);
		}
		void OnExecute(float elapsedTime) override
		{
            // アニメーションが終了していたら遷移
            if (!owner->GetAnimator()->IsPlayAnimation())
                owner->GetStateMachine().ChangeSubState(Network::GetPlayerSubStateName(Network::PlayerSubStates::Guarding));
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
		const char* GetName() const override { return Network::GetPlayerSubStateName(Network::PlayerSubStates::Guarding); }
		void OnEnter() override
		{
			owner->GetAnimator()->PlayAnimation(u8"Guard", true, 0.2f);
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

const char* PlayerGuardState::GetName() const
{
    return Network::GetPlayerMainStateName(Network::PlayerMainStates::Guard);
}

void PlayerGuardState::OnEnter()
{
    // フラグを立てる
    owner->SetIsGuard(true);

	owner->GetAnimator()->SetRootNodeIndex("ORG-hips");
	owner->GetAnimator()->SetIsUseRootMotion(true);
	owner->GetAnimator()->SetRootMotionOption(Animator::RootMotionOption::RemovePositionXY);
	// 初期サブステート設定
	ChangeSubState(Network::GetPlayerSubStateName(Network::PlayerSubStates::GuardStart));
}

void PlayerGuardState::OnExecute(float elapsedTime)
{
    // 移動方向に向く
    owner->RotationMovement(elapsedTime);

    // 攻撃移行
    if (owner->IsAttack())
        owner->GetStateMachine().ChangeState(Network::GetPlayerMainStateName(Network::PlayerMainStates::Attack1));
    // 回避移行
    else if (owner->IsEvade())
        owner->GetStateMachine().ChangeState(Network::GetPlayerMainStateName(Network::PlayerMainStates::Evade));
    // ガード解除
    else if (!owner->IsGuard())
        owner->GetStateMachine().ChangeState(Network::GetPlayerMainStateName(Network::PlayerMainStates::Idle));
}

void PlayerGuardState::OnExit()
{
    // フラグを下ろす
    owner->SetIsGuard(false);
}

#pragma endregion

#pragma region 被弾
const char* PlayerHitState::GetName() const
{
    return Network::GetPlayerMainStateName(Network::PlayerMainStates::Hit);
}
void PlayerHitState::OnEnter()
{
	static const std::string HitAnimationNames[] =
	{
		u8"Hit1",
		u8"Hit2",
		u8"Hit3",
		u8"Hit4",
		u8"Hit5",
	};

	owner->GetAnimator()->SetRootNodeIndex("ORG-hips");
	owner->GetAnimator()->SetIsUseRootMotion(true);
	owner->GetAnimator()->SetRootMotionOption(Animator::RootMotionOption::RemovePositionXY);
    owner->GetAnimator()->PlayAnimation(HitAnimationNames[std::rand() % _countof(HitAnimationNames)], false, 0.2f);

    // 被弾モーション中は押し出されないようにする
    auto charactorController = owner->GetPlayer()->GetCharactorController();
	if (charactorController != nullptr)
	{
		charactorController->SetIsPushable(false);
	}
}

void PlayerHitState::OnExecute(float elapsedTime)
{
    // アニメーションが終了していたら遷移
    if (!owner->GetAnimator()->IsPlayAnimation())
        owner->GetStateMachine().ChangeState(Network::GetPlayerMainStateName(Network::PlayerMainStates::Idle));
}

void PlayerHitState::OnExit()
{
    // 押し出されれるようにする
    auto charactorController = owner->GetPlayer()->GetCharactorController();
    if (charactorController != nullptr)
    {
        charactorController->SetIsPushable(true);
    }
}

const char* PlayerHitKnockDownState::GetName() const
{
    return Network::GetPlayerMainStateName(Network::PlayerMainStates::HitKnockDown);
}

void PlayerHitKnockDownState::OnEnter()
{
    owner->GetAnimator()->SetRootNodeIndex("ORG-hips");
    owner->GetAnimator()->SetIsUseRootMotion(true);
    owner->GetAnimator()->SetRootMotionOption(Animator::RootMotionOption::RemovePositionXY);
    owner->GetAnimator()->PlayAnimation("Hit_knockdown", false, 0.2f);

    // 被弾モーション中は押し出されないようにする
    auto charactorController = owner->GetPlayer()->GetCharactorController();
    if (charactorController != nullptr)
    {
        charactorController->SetIsPushable(false);
    }
}

void PlayerHitKnockDownState::OnExecute(float elapsedTime)
{
    // アニメーションが終了していたら遷移
    if (!owner->GetAnimator()->IsPlayAnimation())
        owner->GetStateMachine().ChangeState(Network::GetPlayerMainStateName(Network::PlayerMainStates::Idle));
}

void PlayerHitKnockDownState::OnExit()
{
    // 押し出されれるようにする
    auto charactorController = owner->GetPlayer()->GetCharactorController();
    if (charactorController != nullptr)
    {
        charactorController->SetIsPushable(true);
    }
}

const char* PlayerGuardHitState::GetName() const
{
    return Network::GetPlayerMainStateName(Network::PlayerMainStates::GuardHit);
}

void PlayerGuardHitState::OnEnter()
{
    static const std::string GuardHitAnimationNames[] =
    {
        u8"Guard_Hit1",
        u8"Guard_Hit2",
        u8"Guard_Hit3",
    };

    owner->GetAnimator()->SetRootNodeIndex("ORG-hips");
    owner->GetAnimator()->SetIsUseRootMotion(true);
    owner->GetAnimator()->SetRootMotionOption(Animator::RootMotionOption::RemovePositionXY);
    owner->GetAnimator()->PlayAnimation(GuardHitAnimationNames[std::rand() % _countof(GuardHitAnimationNames)], false, 0.2f);

    // 被弾モーション中は押し出されないようにする
    auto charactorController = owner->GetPlayer()->GetCharactorController();
    if (charactorController != nullptr)
    {
        charactorController->SetIsPushable(false);
    }
}

void PlayerGuardHitState::OnExecute(float elapsedTime)
{
    // アニメーションが終了していたら遷移
    if (!owner->GetAnimator()->IsPlayAnimation())
        owner->GetStateMachine().ChangeState(Network::GetPlayerMainStateName(Network::PlayerMainStates::Idle));
}

void PlayerGuardHitState::OnExit()
{
    // 押し出されれるようにする
    auto charactorController = owner->GetPlayer()->GetCharactorController();
    if (charactorController != nullptr)
    {
        charactorController->SetIsPushable(true);
    }
}

const char* PlayerGuardBreakState::GetName() const
{
    return Network::GetPlayerMainStateName(Network::PlayerMainStates::GuardBreak);
}

void PlayerGuardBreakState::OnEnter()
{
    owner->GetAnimator()->SetRootNodeIndex("ORG-hips");
    owner->GetAnimator()->SetIsUseRootMotion(true);
    owner->GetAnimator()->SetRootMotionOption(Animator::RootMotionOption::RemovePositionXY);
    owner->GetAnimator()->PlayAnimation("Guard_Break", false, 0.2f);

    // 被弾モーション中は押し出されないようにする
    auto charactorController = owner->GetPlayer()->GetCharactorController();
    if (charactorController != nullptr)
    {
        charactorController->SetIsPushable(false);
    }
}

void PlayerGuardBreakState::OnExecute(float elapsedTime)
{
    // アニメーションが終了していたら遷移
    if (!owner->GetAnimator()->IsPlayAnimation())
        owner->GetStateMachine().ChangeState(Network::GetPlayerMainStateName(Network::PlayerMainStates::Idle));
}

void PlayerGuardBreakState::OnExit()
{
    // 押し出されれるようにする
    auto charactorController = owner->GetPlayer()->GetCharactorController();
    if (charactorController != nullptr)
    {
        charactorController->SetIsPushable(true);
    }
}

#pragma endregion

#pragma region 死亡
const char* PlayerDeathState::GetName() const
{
    return Network::GetPlayerMainStateName(Network::PlayerMainStates::Death);
}
void PlayerDeathState::OnEnter()
{
    static const std::string DeathAnimationNames[] =
    {
        u8"Death",
        u8"Death2",
        u8"Hit_knockdown_Death",
    };

    owner->GetAnimator()->SetRootNodeIndex("ORG-hips");
    owner->GetAnimator()->SetIsUseRootMotion(true);
    owner->GetAnimator()->SetRootMotionOption(Animator::RootMotionOption::RemovePositionXY);
    owner->GetAnimator()->PlayAnimation(DeathAnimationNames[std::rand() % _countof(DeathAnimationNames)], false, 0.2f);
}

void PlayerDeathState::OnExecute(float elapsedTime)
{
}
#pragma endregion

#pragma endregion
