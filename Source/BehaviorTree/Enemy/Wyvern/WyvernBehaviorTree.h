#pragma once

#include <memory>
#include "../../Library/Algorithm/BehaviorTree/BehaviorTree.h"

// 前方宣言
class WyvernEnemyController;
class Animator;

class WyvernBehaviorTree
{
public:
	WyvernBehaviorTree();

private:
	// ビヘイビアツリー
	std::unique_ptr<BehaviorTree<WyvernBehaviorTree>> behaviorTree;
	// ビヘイビアデータ
	std::unique_ptr < BehaviorData<WyvernBehaviorTree>> behaviorData;
	// ビヘイビアノード
	std::unique_ptr < BehaviorNodeBase<WyvernBehaviorTree>> behaviorNode;
};