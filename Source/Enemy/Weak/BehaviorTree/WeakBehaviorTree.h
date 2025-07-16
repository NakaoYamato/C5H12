#pragma once

#include <memory>
#include "../../Library/Component/BehaviorController.h"
#include "../StateMachine/WeakStateMachine.h"
#include "../../Source/AI/MetaAI.h"

class WeakBehaviorTree : public BehaviorTree
{
public:
	WeakBehaviorTree(
		WeakStateMachine* stateMachine,
		Animator* animator,
		MetaAI* metaAI);

	// 開始処理
	void Start() override;
	// ビヘイビアツリー実行
	void Execute(float elapsedTime) override;
	// GUI描画
	void DrawGui() override;
#pragma region アクセサ
	// ステートマシンを取得
	WeakStateMachine* GetStateMachine() { return _stateMachine; }
	Animator* GetAnimator() { return _animator; }
	MetaAI* GetMetaAI() { return _metaAI; }
#pragma endregion
private:
	// ビヘイビアツリーのGUI描画
	void DrawBehaviorTreeGui(BehaviorNodeBase<WeakBehaviorTree>* node);
private:
	// ビヘイビアツリー
	std::unique_ptr<BehaviorTreeBase<WeakBehaviorTree>> _behaviorTree;
	// ビヘイビアデータ
	std::unique_ptr<BehaviorData<WeakBehaviorTree>> _behaviorData;
	// 起動中のビヘイビアノード
	BehaviorNodeBase<WeakBehaviorTree>* _activeNode = nullptr;
	WeakStateMachine* _stateMachine = nullptr;
	Animator* _animator = nullptr;
	MetaAI* _metaAI = nullptr;
};