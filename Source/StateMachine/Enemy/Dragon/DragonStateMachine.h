#pragma once

#include <variant>
#include "../../Library/Algorithm/StateMachine/StateMachine.h"

// 前方宣言
class EnemyController;
class Animator;

class DragonStateMachine
{
public:
	DragonStateMachine(EnemyController* enemy, Animator* animator);
	~DragonStateMachine() {}

	// 実行処理
	void Execute(float elapsedTime);
	// Gui描画
	void DrawGui();

#pragma region アクセサ
	StateMachine<DragonStateMachine>& GetStateMachine() { return _stateMachine; }
	EnemyController* GetEnemy() { return _enemy; }
	Animator* GetAnimator() { return _animator; }

	// ステート変更
	//void ChangeState(enum class PlayerMainStates mainStateName, enum class PlayerSubStates subStateName);
	// ステート名取得
	const char* GetStateName() { return _stateMachine.GetState()->GetName(); }
	// サブステート名取得
	const char* GetSubStateName() { return _stateMachine.GetState()->GetSubStateName(); }

	// キャンセルイベントを取得
	bool CallCancelEvent() const { return _callCancelEvent; }
#pragma endregion
private:
	StateMachine<DragonStateMachine> _stateMachine;
	EnemyController* _enemy = nullptr;
	Animator* _animator = nullptr;

	bool _callCancelEvent = false;
};

#pragma region 各ステート
#pragma region テスト用
class DragonAttackMedleyState final : public HierarchicalStateBase<DragonStateMachine>
{
public:
	DragonAttackMedleyState(DragonStateMachine* stateMachine) : HierarchicalStateBase(stateMachine) {}
	~DragonAttackMedleyState() override {}
	// ステート名取得
	const char* GetName() const override { return "AttackMedley"; }
	void OnEnter() override;
	void OnExecute(float elapsedTime) override;
	void OnExit() override {}
};
#pragma endregion


#pragma endregion