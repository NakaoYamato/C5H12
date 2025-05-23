#include "WyvernBehaviorTree.h"

#include "../../Source/Component/Enemy/Wyvern/WyvernEnemyController.h"
#include "WyvernActionDerived.h"
#include "WyvernJudgmentDerived.h"

#include <imgui.h>

WyvernBehaviorTree::WyvernBehaviorTree(WyvernEnemyController* wyvern, Animator* animator) :
	_wyvern(wyvern),
	_animator(animator)
{
	_behaviorData = std::make_unique<BehaviorData<WyvernBehaviorTree>>();
	_behaviorTree = std::make_unique<BehaviorTree<WyvernBehaviorTree>>(this);

	_behaviorTree->AddNode("", "Root", 0, SelectRule::Priority, nullptr, nullptr);
	{
		_behaviorTree->AddNode("Root", "Battle", 4, SelectRule::Priority, std::make_shared<WyvernBattleJudgment>(this), nullptr);
		{
			_behaviorTree->AddNode("Battle", "Attack", 1, SelectRule::Random, std::make_shared<WyvernAttackJudgment>(this), nullptr);
			{
				_behaviorTree->AddNode("Attack", "Normal", 1, SelectRule::Non, nullptr, std::make_shared<WyvernNormalAction>(this));
			}
			//_behaviorTree->AddNode("Battle", "Pursuit", 2, SelectRule::Non, nullptr, new PursuitAction(this));
		}
		_behaviorTree->AddNode("Root", "Scout", 5, SelectRule::Priority, nullptr, nullptr);
		{
			_behaviorTree->AddNode("Scout", "Idle", 2, SelectRule::Non, nullptr, std::make_shared<WyvernIdleAction>(this));
		}
	}
}

void WyvernBehaviorTree::Run(float elapsedTime)
{
	// 現在の実行ノードがなければ取得
	if (_activeNode == nullptr)
	{
		// 推論
		_activeNode = _behaviorTree->ActiveNodeInference(_behaviorData.get());
	}
	// 実行ノードがあれば実行
	if (_activeNode != nullptr)
	{
		_activeNode = _behaviorTree->Run(_activeNode, _behaviorData.get(), elapsedTime);
	}
}

void WyvernBehaviorTree::DrawGui()
{	
	// ビヘイビアツリーのGUI描画
	if (_activeNode != nullptr)
	{
		//activeNode->DrawGui();
	}
	ImGui::Text("BehaviorTree");
	if (_activeNode != nullptr)
		ImGui::Text("ActiveNode : %s", _activeNode->GetName());
	//ImGui::Text("SubState : %s", activeNode->GetSubStateName());
	//ImGui::Text("BehaviorData : %s", behaviorData->GetName());
	//ImGui::Text("BehaviorTree : %s", behaviorTree->GetName());
}
