#include "WeakStateMachine.h"

#include "../../Library/Component/Animator.h"
#include "../../EnemyController.h"
#include "WeakStates.h"
#include "../WeakController.h"

#include <imgui.h>

WeakStateMachine::WeakStateMachine(Actor* owner)
{
	// コンポーネント取得
	_enemy = owner->GetComponent<EnemyController>().get();
	_weak = owner->GetComponent<WeakController>().get();
	_animator = owner->GetComponent<Animator>().get();
	_damageable = owner->GetComponent<Damageable>().get();
	_combatStatus = owner->GetComponent<CombatStatusController>().get();

	// ステートの登録
	_stateMachine.RegisterState(std::make_unique<WeakIdleState>(this));
	_stateMachine.RegisterState(std::make_unique<WeakRestState>(this));
	_stateMachine.RegisterState(std::make_unique<WeakThreatState>(this));
	_stateMachine.RegisterState(std::make_unique<WeakMoveState>(this));
	_stateMachine.RegisterState(std::make_unique<WeakDashState>(this));
	_stateMachine.RegisterState(std::make_unique<WeakAttackState>(this));
	_stateMachine.RegisterState(std::make_unique<WeakJumpAttackState>(this));
	_stateMachine.RegisterState(std::make_unique<WeakDamageState>(this));
	_stateMachine.RegisterState(std::make_unique<WeakDeathState>(this));
}
// 開始処理
void WeakStateMachine::Start()
{
	// 初期ステートを設定
	_stateMachine.ChangeState("Idle");
}
// 実行処理
void WeakStateMachine::Execute(float elapsedTime)
{
	// 被弾処理
	if (_enemy->IsPerformDamageReaction())
	{
		// 被弾遷移
		_stateMachine.ChangeState("Damage");
		_enemy->SetPerformDamageReaction(false);
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
// Gui描画
void WeakStateMachine::DrawGui()
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
// ステート変更
void WeakStateMachine::ChangeState(const char* mainStateName, const char* subStateName)
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
const char* WeakStateMachine::GetStateName()
{
	if (!_stateMachine.GetState())
		return "";

	return _stateMachine.GetState()->GetName();
}
// サブステート名取得
const char* WeakStateMachine::GetSubStateName()
{
	if (!_stateMachine.GetState())
		return "";

	return _stateMachine.GetState()->GetSubStateName();
}
