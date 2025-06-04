#include "WyvernStateMachine.h"

#include "../../Library/Component/Animator.h"
#include "../WyvernEnemyController.h"
#include "WyvernMainStates.h"

#include <imgui.h>

WyvernStateMachine::WyvernStateMachine(WyvernEnemyController* wyvern, Animator* animator, Damageable* damageable) :
	_wyvern(wyvern),
	_animator(animator),
	_damageable(damageable)
{
	// ステートの登録
	_stateMachine.RegisterState(std::make_unique<WyvernIdleState>(this));

	_stateMachine.RegisterState(std::make_unique<WyvernToTargetState>(this));

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
    _fireBreath = false;

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
            // ブレス攻撃判定
			else if (animationEvent.GetMessageList().at(event.messageIndex) == "FireBreath")
			{
				_fireBreath = true;
			}
        }
    }

    // 被弾処理
	if (_wyvern->IsPerformDamageReaction())
	{
        // 被弾遷移
		_stateMachine.ChangeState("Damage");
        _wyvern->SetPerformDamageReaction(false);
	}

    // 死亡処理
    if (GetDamageable()->IsDead() && _stateMachine.GetStateName() != "Death")
    {
        // 死亡遷移
		_stateMachine.ChangeState("Death");
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

const char* WyvernStateMachine::GetStateName()
{
    if (!_stateMachine.GetState())
		return "";

    return _stateMachine.GetState()->GetName();
}

const char* WyvernStateMachine::GetSubStateName()
{
    if (!_stateMachine.GetState())
        return "";

    return _stateMachine.GetState()->GetSubStateName();
}
