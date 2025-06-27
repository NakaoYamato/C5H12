#pragma once

#include <memory>
#include "../../Library/Component/BehaviorController.h"
#include "../StateMachine/WyvernStateMachine.h"

class WyvernBehaviorTree : public BehaviorTree
{
public:
	WyvernBehaviorTree(
		WyvernStateMachine* stateMachine,
		Animator* animator);

	// 開始処理
	void Start() override;
	// ビヘイビアツリー実行
	void Execute(float elapsedTime) override;
	// GUI描画
	void DrawGui() override;

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
	std::unique_ptr<BehaviorTreeBase<WyvernBehaviorTree>> _behaviorTree;
	// ビヘイビアデータ
	std::unique_ptr<BehaviorData<WyvernBehaviorTree>> _behaviorData;
	// 起動中のビヘイビアノード
	BehaviorNodeBase<WyvernBehaviorTree>* _activeNode = nullptr;

	WyvernStateMachine* _stateMachine = nullptr;
	Animator*				_animator = nullptr;
	bool					_callCancelEvent = false;
};