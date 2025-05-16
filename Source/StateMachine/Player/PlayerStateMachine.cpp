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
    _stateMachine.RegisterState(std::make_shared<PlayerAttack1State>(this));

    // 初期ステート設定
    _stateMachine.ChangeState("Idle");
}

// 実行処理
void PlayerStateMachine::Execute(float elapsedTime)
{
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
    if (owner->GetPlayer()->GetState() == PlayerState::Attack1)
        owner->GetStateMachine().ChangeState("Attack1");
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
            // アニメーションイベントから攻撃キャンセル取得
			auto& animationEvent = owner->GetAnimator()->GetAnimationEvent();
			int massageListSize = (int)animationEvent.GetMessageList().size();
			auto events = owner->GetAnimator()->GetCurrentEvents();
            for (auto& event : events)
            {
                // メッセージインデックスが範囲外ならcontinue
                if (event.messageIndex < 0 || event.messageIndex >= massageListSize)
                    continue;

                // 攻撃キャンセル判定
				if (animationEvent.GetMessageList().at(event.messageIndex) == "Cancel")
				{
					// 攻撃キャンセルがあったらCombo2に遷移
					owner->GetStateMachine().ChangeSubState("Combo2");
				}
            }
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
            // アニメーションイベントから攻撃キャンセル取得
			auto& animationEvent = owner->GetAnimator()->GetAnimationEvent();
            int massageListSize = (int)animationEvent.GetMessageList().size();
			auto events = owner->GetAnimator()->GetCurrentEvents();
            for (auto& event : events)
            {
                // メッセージインデックスが範囲外ならcontinue
                if (event.messageIndex < 0 || event.messageIndex >= massageListSize)
                    continue;

                // 攻撃キャンセル判定
                if (animationEvent.GetMessageList().at(event.messageIndex) == "Cancel")
                {
                    // 攻撃キャンセルがあったらCombo3に遷移
                    owner->GetStateMachine().ChangeSubState("Combo3");
                }
            }
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
            // アニメーションイベントから攻撃キャンセル取得
			auto& animationEvent = owner->GetAnimator()->GetAnimationEvent();
            int massageListSize = (int)animationEvent.GetMessageList().size();
			auto events = owner->GetAnimator()->GetCurrentEvents();
            for (auto& event : events)
            {
                // メッセージインデックスが範囲外ならcontinue
                if (event.messageIndex < 0 || event.messageIndex >= massageListSize)
                    continue;

                // 攻撃キャンセル判定
                if (animationEvent.GetMessageList().at(event.messageIndex) == "Cancel")
                {
                    // 攻撃キャンセルがあったらCombo4に遷移
                    owner->GetStateMachine().ChangeSubState("Combo4");
                }
            }
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
            // アニメーションイベントから攻撃キャンセル取得
			auto& animationEvent = owner->GetAnimator()->GetAnimationEvent();
            int massageListSize = (int)animationEvent.GetMessageList().size();
			auto events = owner->GetAnimator()->GetCurrentEvents();
            for (auto& event : events)
            {
                // メッセージインデックスが範囲外ならcontinue
                if (event.messageIndex < 0 || event.messageIndex >= massageListSize)
                    continue;

                // 攻撃キャンセル判定
                if (animationEvent.GetMessageList().at(event.messageIndex) == "Cancel")
                {
                    // 攻撃キャンセルがあったらCombo5に遷移
                    //owner->GetStateMachine().ChangeSubState("Combo5");
                }
            }
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
    // アニメーションが終了していたら遷移
    if (!owner->GetAnimator()->IsPlayAnimation())
    {
        owner->GetPlayer()->SetState(PlayerState::Idle);
        // 攻撃からIdleに遷移
        owner->GetStateMachine().ChangeState("Idle");
    }
}
#pragma endregion

#pragma endregion

