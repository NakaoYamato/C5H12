#pragma once

#include <memory>
#include "../../Library/Component/BehaviorController.h"
#include "../StateMachine/WeakStateMachine.h"
#include "../../Source/AI/MetaAI.h"
#include "../../Source/Common/CombatStatusController.h"

class WeakBehaviorTree : public BehaviorTree
{
public:
	WeakBehaviorTree(
		WeakStateMachine* stateMachine,
		Actor* owner,
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
	// アニメーターを取得
	Animator* GetAnimator() { return _animator; }
	// 戦闘状態を取得	
	CombatStatusController* GetCombatStatus() { return _combatStatus; }
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
	CombatStatusController* _combatStatus = nullptr;
	MetaAI* _metaAI = nullptr;
};