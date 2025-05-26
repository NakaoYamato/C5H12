#include "WyvernStateMachine.h"

#include "../../Library/Component/Animator.h"
#include "../WyvernEnemyController.h"
#include "WyvernMainStates.h"

#include <imgui.h>

WyvernStateMachine::WyvernStateMachine(WyvernEnemyController* wyvern, Animator* animator) :
	_wyvern(wyvern),
	_animator(animator)
{
	// ステートの登録
	_stateMachine.RegisterState(std::make_unique<WyvernIdleState>(this));

	_stateMachine.RegisterState(std::make_unique<WyvernBiteAttackState>(this));
	_stateMachine.RegisterState(std::make_unique<WyvernClawAttackState>(this));
	_stateMachine.RegisterState(std::make_unique<WyvernTailAttackState>(this));
	_stateMachine.RegisterState(std::make_unique<WyvernBreathAttackState>(this));

	_stateMachine.RegisterState(std::make_unique<WyvernBackStepState>(this));
	_stateMachine.RegisterState(std::make_unique<WyvernPursuitState>(this));

	_stateMachine.RegisterState(std::make_unique<WyvernDamageState>(this));
	_stateMachine.RegisterState(std::make_unique<WyvernDeathState>(this));

	// 初期ステートを設定
	_stateMachine.ChangeState("Idle");
}

void WyvernStateMachine::Execute(float elapsedTime)
{
    _callCancelEvent = false;

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

            // キャンセル判定
            if (animationEvent.GetMessageList().at(event.messageIndex) == "Cancel")
            {
                _callCancelEvent = true;
            }
        }
    }
    // 被弾処理
	if (_wyvern->GetDamageCounter() > 0)
	{
        // 被弾遷移
		_stateMachine.ChangeState("Damage");
        _wyvern->SetDamageCounter(0.0f);
	}

	// ステートマシンの実行
    _stateMachine.Update(elapsedTime);
}

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

void WyvernStateMachine::ChangeState(const char* mainStateName, const char* subStateName)
{
}
