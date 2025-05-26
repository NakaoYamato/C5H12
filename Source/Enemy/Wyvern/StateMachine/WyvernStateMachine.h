#pragma once

#include <variant>
#include "../../Library/Algorithm/StateMachine/StateMachine.h"
#include "../../Library/Math/Vector.h"

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

	// ステート変更
	void ChangeState(const char* mainStateName, const char* subStateName);
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

	bool					_callCancelEvent = false;
};
