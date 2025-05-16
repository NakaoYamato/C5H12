#include "PlayerStateMachine.h"

#include "../../Library/Component/Animator.h"
#include "../../Source/Component/Player/PlayerController.h"

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

    // 初期ステート設定
    _stateMachine.ChangeState("Idle");
}

// 実行処理
void PlayerStateMachine::Execute(float elapsedTime)
{
    _callCancelEvent = false;

    // アニメーションイベント取得
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

#pragma region 各ステート
#pragma region 待機
void PlayerIdleState::OnEnter()
{
    owner->GetAnimator()->PlayAnimation(u8"Idle1", true, 0.2f);
    owner->GetAnimator()->SetIsUseRootMotion(false);
}

void PlayerIdleState::OnExecute(float elapsedTime)
{
    if (owner->GetPlayer()->IsAttack())
        owner->GetStateMachine().ChangeState("Attack1");
	else if (owner->GetPlayer()->IsMoving())
        owner->GetStateMachine().ChangeState("Run");
    // 回避移行
    else if (owner->GetPlayer()->IsEvade())
        owner->GetStateMachine().ChangeState("Evade");
}
#pragma endregion

#pragma region 走り
void PlayerRunState::OnEnter()
{
    owner->GetAnimator()->SetRootNodeIndex("root");
    owner->GetAnimator()->SetIsUseRootMotion(true);
    owner->GetAnimator()->SetRootMotionOption(Animator::RootMotionOption::RemovePositionXY);
    owner->GetAnimator()->PlayAnimation(u8"Run_Forward_Rootmotion", true, 0.2f);
}
void PlayerRunState::OnExecute(float elapsedTime)
{
    if (owner->GetPlayer()->IsAttack())
        owner->GetStateMachine().ChangeState("Attack1");
    else if (owner->GetPlayer()->IsDush())
        owner->GetStateMachine().ChangeState("Sprint");
    else if (!owner->GetPlayer()->IsMoving())
        owner->GetStateMachine().ChangeState("Idle");
    // 回避移行
	else if (owner->GetPlayer()->IsEvade())
		owner->GetStateMachine().ChangeState("Evade");
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
            owner->GetAnimator()->PlayAnimation(u8"Sprint_Start_no_weapon_Rootmotion", false, 0.2f);
        }
        void OnExecute(float elapsedTime) override
        {
            // 攻撃移行
            if (owner->GetPlayer()->IsAttack())
                owner->GetStateMachine().ChangeSubState("SprintAttack");
            // 回避移行
            else if (owner->GetPlayer()->IsEvade())
                owner->GetStateMachine().ChangeState("Evade");
            // アニメーションが終了していたら遷移
            else if (!owner->GetAnimator()->IsPlayAnimation())
            {
                owner->GetStateMachine().ChangeSubState("Sprinting");
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
            owner->GetAnimator()->SetRootNodeIndex("root");
            owner->GetAnimator()->SetIsUseRootMotion(true);
            owner->GetAnimator()->SetRootMotionOption(Animator::RootMotionOption::RemovePositionXY);
            owner->GetAnimator()->PlayAnimation(u8"Sprint_no_weapon_Rootmotion", true, 0.2f);
        }
        void OnExecute(float elapsedTime) override
        {
			// 攻撃移行
            if (owner->GetPlayer()->IsAttack())
                owner->GetStateMachine().ChangeSubState("SprintAttack");
            // 回避移行
            else if (owner->GetPlayer()->IsEvade())
                owner->GetStateMachine().ChangeState("Evade");
            // ダッシュ解除で移動に遷移
            else if (!owner->GetPlayer()->IsDush())
                owner->GetStateMachine().ChangeState("Run");
        }
        void OnExit() override
        {
        }
    };
    class SprintAttackSubState : public StateBase<PlayerStateMachine>
    {
    public:
        SprintAttackSubState(PlayerStateMachine* stateMachine) : StateBase(stateMachine)
        {
        }
        const char* GetName() const override { return "SprintAttack"; }

        void OnEnter() override
        {
            owner->GetAnimator()->SetRootNodeIndex("ORG-hips");
            owner->GetAnimator()->SetIsUseRootMotion(true);
            owner->GetAnimator()->SetRootMotionOption(Animator::RootMotionOption::RemovePositionXY);
            owner->GetAnimator()->PlayAnimation(u8"Attack_Sprint", false, 0.2f);
        }
        void OnExecute(float elapsedTime) override
        {
            if (owner->CallCancelEvent())
            {
                // キャンセル攻撃
                if (owner->GetPlayer()->IsAttack())
                    owner->GetStateMachine().ChangeState("Attack1");
                // 回避移行
                else if (owner->GetPlayer()->IsEvade())
                    owner->GetStateMachine().ChangeState("Evade");
            }
            // アニメーションが終了していたら遷移
            else if (!owner->GetAnimator()->IsPlayAnimation())
                owner->GetStateMachine().ChangeState("Idle");
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
    owner->GetAnimator()->SetRootNodeIndex("root");
    owner->GetAnimator()->SetIsUseRootMotion(true);
    owner->GetAnimator()->SetRootMotionOption(Animator::RootMotionOption::RemovePositionXY);
    // 初期サブステート設定
    ChangeSubState("SprintStart");
}

void PlayerSprintState::OnExecute(float elapsedTime)
{
}
#pragma endregion

#pragma region 回避
void PlayerEvadeState::OnEnter()
{
    static const std::string evadeAnimationNames[] =
    {
        u8"Evade_Forward",
        u8"Evade_Forward_Left",
        u8"Evade_Left",
		u8"Evade_Backward_Left",
		u8"Evade_Backward",
		u8"Evade_Backward_Right",
		u8"Evade_Right",
		u8"Evade_Forward_Right"
    };

    owner->GetAnimator()->SetRootNodeIndex("ORG-hips");
    owner->GetAnimator()->SetIsUseRootMotion(true);
	owner->GetAnimator()->SetRootMotionOption(Animator::RootMotionOption::RemovePositionXY);
	// 入力方向から回避方向を決定
	std::string evadeAnimationName = evadeAnimationNames[0];
	auto playerInput = owner->GetPlayer()->GetPlayerInput();
    if (playerInput)
    {
        Vector2 inputAxis = playerInput->GetLAxisValue();
		// 入力方向が0なら前転
        if (inputAxis.LengthSq() == 0.0f)
            evadeAnimationName = evadeAnimationNames[0];
        else
        {
            // 入力方向とプレイヤーのY軸回転量から回避方向を決定
            float angle = DirectX::XMConvertToDegrees(atan2f(inputAxis.x, inputAxis.y));
			if (angle < 0.0f)
				angle += 360.0f;
			// 360度を8方向に分割
			int index = (int)(angle / 45.0f + 0.5f);
			if (index >= 8)
				index = 0;
			evadeAnimationName = evadeAnimationNames[index];
        }
    }

	owner->GetAnimator()->PlayAnimation(evadeAnimationName, false, 0.2f);
}
void PlayerEvadeState::OnExecute(float elapsedTime)
{
    // アニメーションが終了していたら遷移
	if (!owner->GetAnimator()->IsPlayAnimation())
	{
		owner->GetStateMachine().ChangeState("Idle");
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
        Combo1SubState(PlayerStateMachine* stateMachine) : StateBase(stateMachine)
        {
        }
        const char* GetName() const override { return "Combo1"; }

        void OnEnter() override
        {
            owner->GetAnimator()->PlayAnimation(u8"Attack_Combo1", false, 0.2f);
        }
        void OnExecute(float elapsedTime) override
        {
            // 攻撃キャンセル判定
            if (owner->CallCancelEvent())
                if (owner->GetPlayer()->IsAttack())
                    owner->GetStateMachine().ChangeSubState("Combo2");
        }
        void OnExit() override 
        {
        }
    };
    class Combo2SubState : public StateBase<PlayerStateMachine>
    {
    public:
        Combo2SubState(PlayerStateMachine* stateMachine) : StateBase(stateMachine)
        {
        }
        const char* GetName() const override { return "Combo2"; }

        void OnEnter() override
        {
            owner->GetAnimator()->PlayAnimation(u8"Attack_Combo2", false, 0.3f);
        }
        void OnExecute(float elapsedTime) override
        {
            // 攻撃キャンセル判定
            if (owner->CallCancelEvent())
                if (owner->GetPlayer()->IsAttack())
                    owner->GetStateMachine().ChangeSubState("Combo3");
        }
        void OnExit() override 
        {
        }
    };
    class Combo3SubState : public StateBase<PlayerStateMachine>
    {
    public:
        Combo3SubState(PlayerStateMachine* stateMachine) : StateBase(stateMachine)
        {
        }
        const char* GetName() const override { return "Combo3"; }

        void OnEnter() override
        {
            owner->GetAnimator()->PlayAnimation(u8"Attack_Combo3", false, 0.3f);
        }
        void OnExecute(float elapsedTime) override
        {
            // 攻撃キャンセル判定
            if (owner->CallCancelEvent())
                if (owner->GetPlayer()->IsAttack())
                    owner->GetStateMachine().ChangeSubState("Combo4");
        }
        void OnExit() override 
        {
        }
    };
    class Combo4SubState : public StateBase<PlayerStateMachine>
    {
    public:
        Combo4SubState(PlayerStateMachine* stateMachine) : StateBase(stateMachine)
        {
        }
        const char* GetName() const override { return "Combo4"; }

        void OnEnter() override
        {
            owner->GetAnimator()->PlayAnimation(u8"Attack_Combo4", false, 0.3f);
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
    owner->GetAnimator()->SetRootNodeIndex("ORG-hips");
    owner->GetAnimator()->SetIsUseRootMotion(true);
    owner->GetAnimator()->SetRootMotionOption(Animator::RootMotionOption::RemovePositionXY);
    // 初期サブステート設定
	ChangeSubState("Combo1");
}

void PlayerAttack1State::OnExecute(float elapsedTime)
{
    // 攻撃キャンセル判定
    if (owner->CallCancelEvent())
        // 回避移行
        if (owner->GetPlayer()->IsEvade())
            owner->GetStateMachine().ChangeState("Evade");

    // アニメーションが終了していたら遷移
    else if (!owner->GetAnimator()->IsPlayAnimation())
    {
        owner->GetPlayer()->SetState(PlayerState::Idle);
        // 攻撃からIdleに遷移
        owner->GetStateMachine().ChangeState("Idle");
    }
}
#pragma endregion

#pragma endregion
