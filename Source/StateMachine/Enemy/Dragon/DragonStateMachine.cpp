#include "DragonStateMachine.h"

#include "../../Library/Component/Animator.h"
#include "../../Source/Component/Enemy/EnemyController.h"
#include <imgui.h>

DragonStateMachine::DragonStateMachine(EnemyController* enemy, Animator* animator) :
	_enemy(enemy),
	_animator(animator)
{
	// ステート設定
	_stateMachine.RegisterState(std::make_shared<DragonAttackMedleyState>(this));
	//_stateMachine.RegisterState(std::make_shared<DragonIdleState>(this));
	//_stateMachine.RegisterState(std::make_shared<DragonRunState>(this));
	//_stateMachine.RegisterState(std::make_shared<DragonAttack1State>(this));
	//_stateMachine.RegisterState(std::make_shared<DragonHitState>(this));
	//_stateMachine.RegisterState(std::make_shared<DragonDeathState>(this));

	// 初期ステート設定
	_stateMachine.ChangeState("AttackMedley");
}

// 実行処理
void DragonStateMachine::Execute(float elapsedTime)
{
	_stateMachine.Update(elapsedTime);
}

// Gui描画
void DragonStateMachine::DrawGui()
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
void DragonAttackMedleyState::OnEnter()
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
        0.2f);
}

void DragonAttackMedleyState::OnExecute(float elapsedTime)
{
    if (!owner->GetAnimator()->IsPlayAnimation())
    {
        owner->GetStateMachine().ChangeState(GetName());
    }
}
#pragma endregion

#pragma endregion
