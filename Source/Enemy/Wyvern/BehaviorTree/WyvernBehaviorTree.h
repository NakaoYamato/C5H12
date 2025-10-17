#pragma once

#include <memory>
#include "../../Library/Component/BehaviorController.h"
#include "../StateMachine/WyvernStateMachine.h"
#include "../../Source/Common/CombatStatusController.h"

class WyvernBehaviorTree : public BehaviorTree
{
public:
	enum Desire
	{
		NearAttack,
		FarAttack,
		Flight,
		TakeDistance,
		ShortBreak,

		DesireCount
	};

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

#pragma region アクセサ
	// ステートマシンを取得
	WyvernStateMachine* GetStateMachine() { return _stateMachine; }
	// アニメーターを取得
	Animator* GetAnimator() { return _animator; }
	// 戦闘状態を取得	
	CombatStatusController* GetCombatStatus() { return _combatStatus; }
	// キャンセルイベントを取得
	bool CallCancelEvent() const { return _callCancelEvent; }
#pragma endregion

private:
	// ビヘイビアツリーのGUI描画
	void DrawBehaviorTreeGui(BehaviorNodeBase<WyvernBehaviorTree>* node);

private:
	// ビヘイビアツリー
	std::unique_ptr<BehaviorTreeBase<WyvernBehaviorTree>> _behaviorTree;
	// ビヘイビアデータ
	std::unique_ptr<BehaviorData<WyvernBehaviorTree>> _behaviorData;
	// 起動中のビヘイビアノード
	BehaviorNodeBase<WyvernBehaviorTree>* _activeNode = nullptr;

	WyvernStateMachine*		_stateMachine = nullptr;
	Animator*				_animator = nullptr;
	CombatStatusController*	_combatStatus = nullptr;
	bool					_callCancelEvent = false;

	float _desires[static_cast<size_t>(Desire::DesireCount)] = {};
	float _desiresFactor[static_cast<size_t>(Desire::DesireCount)] = { 1.0f, 1.0f, 1.0f, 1.0f, 1.0f };
};