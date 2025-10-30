#pragma once

#include <memory>
#include "../../Library/Component/BehaviorController.h"
#include "../StateMachine/WyvernStateMachine.h"
#include "../../Source/Common/CombatStatusController.h"
#include "../../Source/Common/StaminaController.h"
#include "../../Source/Enemy/BodyPartController.h"

#include "../../Library/Algorithm/BehaviorTree/BehaviorTreeEditor.h"

class WyvernBehaviorTree : public BehaviorTree
{
public:
	// 移動系の消費スタミナ
	static constexpr float MoveStaminaCost = 10.0f;
	// 攻撃系の消費スタミナ
	static constexpr float AttackStaminaCost = 15.0f;
	// ブレス攻撃の消費スタミナ
	static constexpr float BreathAttackStaminaCost = 40.0f;
	// 威嚇によるスタミナ回復量
	static constexpr float ThreatStaminaRecover = 30.0f;

	// 攻撃遷移時の必須スタミナ量
	static constexpr float RequiredStaminaForAttack = 20.0f;

public:
	WyvernBehaviorTree(
		WyvernStateMachine* stateMachine,
		Actor* owner);

	// 開始処理
	void Start() override;
	// ビヘイビアツリー実行
	void Execute(float elapsedTime) override;
	// GUI描画
	void DrawGui() override;
	// 現在の実行ノード名を取得
	std::string GetActiveNodeName() const { return _activeNode ? _activeNode->GetName() : ""; }

#pragma region アクセサ
	// ステートマシンを取得
	WyvernStateMachine* GetStateMachine() { return _stateMachine; }
	// アニメーターを取得
	Animator* GetAnimator() { return _animator; }
	// 戦闘状態を取得	
	CombatStatusController* GetCombatStatus() { return _combatStatus; }
	// スタミナコントローラーを取得
	StaminaController* GetStaminaController() { return _staminaController; }

	// 割り込み処理の名前取得
	const std::string& GetInterruptionName() const { return _interruptionName; }
#pragma endregion

private:
	// ビヘイビアツリーのGUI描画
	void DrawBehaviorTreeGui(BehaviorNodeBase<WyvernBehaviorTree>* node);

	// 被弾割り込み処理が発生しているか
	bool IsDamageInterruption();

private:
	// ビヘイビアツリー
	std::unique_ptr<BehaviorTreeBase<WyvernBehaviorTree>> _behaviorTree;
	// ビヘイビアデータ
	std::unique_ptr<BehaviorData<WyvernBehaviorTree>> _behaviorData;
	// 起動中のビヘイビアノード
	BehaviorNodeBase<WyvernBehaviorTree>* _activeNode = nullptr;

	Actor* 					_owner = nullptr;

	WyvernStateMachine*		_stateMachine = nullptr;
	Animator*				_animator = nullptr;
	CombatStatusController*	_combatStatus = nullptr;
	StaminaController*		_staminaController = nullptr;
	std::vector<BodyPartController*> _bodyPartControllers;

	// 割り込み処理の名前
	std::string _interruptionName = "";
	// 割り込み処理の部位の名前
	std::string _bodyPartName = "";

    BehaviorTreeEditor<WyvernBehaviorTree> _behaviorTreeEditor;
};