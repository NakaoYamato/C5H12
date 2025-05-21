#pragma once

#include "../../Library/Algorithm/StateMachine/StateMachine.h"

// 前方宣言
class WyvernEnemyController;
class Animator;

class WyvernStateMachine
{
public:
	WyvernStateMachine(WyvernEnemyController* wyvern, Animator* animator);
	~WyvernStateMachine() {}
	// 実行処理
	void Execute(float elapsedTime);
	// Gui描画
	void DrawGui();
#pragma region アクセサ
	StateMachine<WyvernStateMachine>& GetStateMachine() { return _stateMachine; }
	WyvernEnemyController* GetWyvern() { return _wyvern; }
	Animator* GetAnimator() { return _animator; }
	// ステート名取得
	const char* GetStateName() { return _stateMachine.GetState()->GetName(); }
	// サブステート名取得
	const char* GetSubStateName() { return _stateMachine.GetState()->GetSubStateName(); }
	// キャンセルイベントを取得
	bool CallCancelEvent() const { return _callCancelEvent; }
#pragma endregion
private:
	StateMachine<WyvernStateMachine> _stateMachine;
	WyvernEnemyController* _wyvern = nullptr;
	Animator* _animator = nullptr;
	bool _callCancelEvent = false;
};

#pragma region 各ステート
#pragma region テスト用
class WyvernAttackMedleyState final : public HierarchicalStateBase<WyvernStateMachine>
{
public:
	WyvernAttackMedleyState(WyvernStateMachine* stateMachine) : HierarchicalStateBase(stateMachine) {}
	~WyvernAttackMedleyState() override {}
	// ステート名取得
	const char* GetName() const override { return "AttackMedley"; }
	void OnEnter() override;
	void OnExecute(float elapsedTime) override;
	void OnExit() override {}
};
#pragma endregion


#pragma endregion