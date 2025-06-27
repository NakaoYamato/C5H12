#pragma once

#include <memory>
#include "../../Library/Algorithm/BehaviorTree/BehaviorTree.h"
#include "../StateMachine/WyvernStateMachine.h"

class WyvernBehaviorTree
{
public:
	WyvernBehaviorTree(
		WyvernStateMachine* stateMachine,
		Animator* animator);

	// ビヘイビアツリー実行
	void Execute(float elapsedTime);
	// GUI描画
	void DrawGui();

#pragma region アクセサ
	// ステートマシンを取得
	WyvernStateMachine* GetStateMachine() { return _stateMachine; }
	Animator* GetAnimator() { return _animator; }
	// キャンセルイベントを取得
	bool CallCancelEvent() const { return _callCancelEvent; }
#pragma endregion

private:
	// ビヘイビアツリーのGUI描画
	void DrawBehaviorTreeGui(BehaviorNodeBase<WyvernBehaviorTree>* node);

private:
	// ビヘイビアツリー
	std::unique_ptr<BehaviorTree<WyvernBehaviorTree>> _behaviorTree;
	// ビヘイビアデータ
	std::unique_ptr<BehaviorData<WyvernBehaviorTree>> _behaviorData;
	// 起動中のビヘイビアノード
	BehaviorNodeBase<WyvernBehaviorTree>* _activeNode = nullptr;

	WyvernStateMachine* _stateMachine = nullptr;
	Animator*				_animator = nullptr;
	bool					_callCancelEvent = false;
};