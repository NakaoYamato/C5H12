#include "WyvernStateMachine.h"

#include "../../Library/Component/Animator.h"
#include "../../Source/Component/Enemy/Wyvern/WyvernEnemyController.h"
#include <imgui.h>

WyvernStateMachine::WyvernStateMachine(WyvernEnemyController* wyvern, Animator* animator) :
	_wyvern(wyvern),
	_animator(animator)
{
	// ステート設定

	// 初期ステート設定
}
// 実行処理
void WyvernStateMachine::Execute(float elapsedTime)
{
	//_stateMachine.Update(elapsedTime);
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
