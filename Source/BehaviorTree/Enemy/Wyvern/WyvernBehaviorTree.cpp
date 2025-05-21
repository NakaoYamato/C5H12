#include "WyvernBehaviorTree.h"

#include "WyvernActionDerived.h"
#include "WyvernJudgmentDerived.h"

WyvernBehaviorTree::WyvernBehaviorTree()
{
	behaviorData = std::make_unique<BehaviorData<WyvernBehaviorTree>>();
	behaviorTree = std::make_unique<BehaviorTree<WyvernBehaviorTree>>(this);

	behaviorTree->AddNode("", "Root", 0, SelectRule::Priority, nullptr, nullptr);
	{

		behaviorTree->AddNode("Root", "Battle", 4, SelectRule::Priority, std::make_shared<WyvernBattleJudgment>(this), nullptr);
	}
}
