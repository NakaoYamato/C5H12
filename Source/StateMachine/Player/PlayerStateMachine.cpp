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
    _stateMachine.SetState("Idle");
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
void PlayerIdleState::Enter()
{
    owner->GetAnimator()->PlayAnimation(u8"Idle1", true);
}

void PlayerIdleState::Execute(float elapsedTime)
{
    if (owner->GetPlayer()->GetState() == PlayerState::Attack1)
        owner->GetStateMachine().SetState("Attack1");
}
#pragma endregion

#pragma region 攻撃1
void PlayerAttack1State::Enter()
{
    owner->GetAnimator()->PlayAnimation(u8"Attack_Combo1", false);
}

void PlayerAttack1State::Execute(float elapsedTime)
{
    // アニメーションが終了していたら遷移
    if (!owner->GetAnimator()->IsPlayAnimation())
    {
        owner->GetPlayer()->SetState(PlayerState::Idle);
        // 攻撃1からIdleに遷移
        owner->GetStateMachine().SetState("Idle");
    }
}
#pragma endregion

#pragma endregion

