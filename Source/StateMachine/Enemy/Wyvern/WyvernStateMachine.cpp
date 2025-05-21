#include "WyvernStateMachine.h"

#include "../../Library/Component/Animator.h"
#include "../../Source/Component/Enemy/Wyvern/WyvernEnemyController.h"
#include <imgui.h>

WyvernStateMachine::WyvernStateMachine(WyvernEnemyController* wyvern, Animator* animator) :
	_wyvern(wyvern),
	_animator(animator)
{
	// ステート設定
    _stateMachine.RegisterState(std::make_shared<WyvernAttackMedleyState>(this));

	// 初期ステート設定
    _stateMachine.ChangeState("AttackMedley");
}
// 実行処理
void WyvernStateMachine::Execute(float elapsedTime)
{
	_stateMachine.Update(elapsedTime);
}
// Gui描画
void WyvernStateMachine::DrawGui()
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

#pragma region 各ステート
#pragma region テスト用
void WyvernAttackMedleyState::OnEnter()
{
    static const std::string AttackAnimationNames[] =
    {
        u8"AttackBiteFront",
        u8"AttackBiteLeft",
        u8"AttackBiteRight",
        u8"AttackFireBall",
        u8"AttackFireBreath",
        u8"AttackFireSwipe",
        u8"AttackSpecial1",
        u8"AttackTailLeft",
        u8"AttackTailRight",
        u8"AttackWingFistLeft",
        u8"AttackWingFistRight",
        u8"AttackWingSwipeLeft",
        u8"AttackWingSwipeRight",
    };

    owner->GetAnimator()->PlayAnimation(
        AttackAnimationNames[std::rand() % _countof(AttackAnimationNames)],
        false,
        0.5f);
}

void WyvernAttackMedleyState::OnExecute(float elapsedTime)
{
    if (!owner->GetAnimator()->IsPlayAnimation())
    {
        owner->GetStateMachine().ChangeState(GetName());
    }
}
#pragma endregion


#pragma endregion
