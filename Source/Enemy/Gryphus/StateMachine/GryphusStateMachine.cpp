#include "GryphusStateMachine.h"

#include "../../Library/Component/Animator.h"
#include "../../EnemyController.h"
#include "GryphusStates.h"

#include <imgui.h>

GryphusStateMachine::GryphusStateMachine(EnemyController* enemy, 
    GryphusController* gryphus, 
    Animator* animator,
    Damageable* damageable) :
    _enemy(enemy),
    _gryphus(gryphus),
    _animator(animator),
    _damageable(damageable)
{
    // ステートの登録
    _stateMachine.RegisterState(std::make_unique<GryphusIdleState>(this));
    _stateMachine.RegisterState(std::make_unique<GryphusRestState>(this));
    _stateMachine.RegisterState(std::make_unique<GryphusThreatState>(this));
    _stateMachine.RegisterState(std::make_unique<GryphusMoveState>(this));
    _stateMachine.RegisterState(std::make_unique<GryphusDashState>(this));
    _stateMachine.RegisterState(std::make_unique<GryphusAttackState>(this));
    _stateMachine.RegisterState(std::make_unique<GryphusDamageState>(this));
    _stateMachine.RegisterState(std::make_unique<GryphusDeathState>(this));

}
// 開始処理
void GryphusStateMachine::Start()
{
    // 初期ステートを設定
    _stateMachine.ChangeState("Idle");
}
// 実行処理
void GryphusStateMachine::Execute(float elapsedTime)
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
void GryphusStateMachine::DrawGui()
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
void GryphusStateMachine::ChangeState(const char* mainStateName, const char* subStateName)
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
const char* GryphusStateMachine::GetStateName()
{
	if (!_stateMachine.GetState())
		return "";

	return _stateMachine.GetState()->GetName();
}
// サブステート名取得
const char* GryphusStateMachine::GetSubStateName()
{
	if (!_stateMachine.GetState())
		return "";

	return _stateMachine.GetState()->GetSubStateName();
}
