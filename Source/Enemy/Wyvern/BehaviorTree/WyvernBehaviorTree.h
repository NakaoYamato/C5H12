#pragma once

#include <memory>
#include "../../Library/Algorithm/BehaviorTree/BehaviorTree.h"

// 前方宣言
class WyvernEnemyController;
class Animator;

class WyvernBehaviorTree
{
public:
	WyvernBehaviorTree(WyvernEnemyController* wyvern, Animator* animator);

	// ビヘイビアツリー実行
	void Execute(float elapsedTime);
	// GUI描画
	void DrawGui();

#pragma region アクセサ
	WyvernEnemyController* GetWyvern() { return _wyvern; }
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

	WyvernEnemyController*	_wyvern = nullptr;
	Animator*				_animator = nullptr;
	bool					_callCancelEvent = false;
};