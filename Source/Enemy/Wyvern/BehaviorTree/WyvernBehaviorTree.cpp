#include "WyvernBehaviorTree.h"

#include "../WyvernEnemyController.h"
#include "WyvernActionDerived.h"
#include "WyvernJudgmentDerived.h"

#include "../../External/nameof/include/nameof.hpp"
#include "../../External/magic_enum/include/magic_enum/magic_enum.hpp"
#include <imgui.h>

WyvernBehaviorTree::WyvernBehaviorTree(WyvernEnemyController* wyvern, Animator* animator) :
	_wyvern(wyvern),
	_animator(animator)
{
	_behaviorData = std::make_unique<BehaviorData<WyvernBehaviorTree>>();
	_behaviorTree = std::make_unique<BehaviorTree<WyvernBehaviorTree>>(this);

	// ビヘイビアツリーを構築
	auto rootNode = _behaviorTree->GetRoot();
	{
		//auto escapeNode = rootNode->AddNode("Escape", 1, SelectRule::Priority, nullptr, nullptr);
		{
			//escapeNode->AddNode("Leave", 0, SelectRule::Non, nullptr, std::make_shared<WyvernLeaveAction>(this));
			//escapeNode->AddNode("LeaveArea", 0, SelectRule::Non, nullptr, std::make_shared<WyvernLeaveAction>(this));
		}

		auto battleNode = rootNode->AddNode("Battle", 4, SelectRule::Priority, std::make_shared<WyvernBattleJudgment>(this), nullptr);
		{
			auto attackNode = battleNode->AddNode("Attack", 1, SelectRule::Priority, std::make_shared<WyvernAttackJudgment>(this), nullptr);
			{
				auto nearAttack = attackNode->AddNode("NearAttack", 1, SelectRule::Random, std::make_shared<WyvernNearAttackJudgment>(this), nullptr);
				{
					nearAttack->AddNode("Bite", 1, SelectRule::Non, nullptr, std::make_shared<WyvernBiteAction>(this));
					nearAttack->AddNode("Claw", 1, SelectRule::Non, nullptr, std::make_shared<WyvernClawAction>(this));
					nearAttack->AddNode("Tail", 1, SelectRule::Non, nullptr, std::make_shared<WyvernTailAction>(this));
					nearAttack->AddNode("BackStep", 1, SelectRule::Non, nullptr, std::make_shared<WyvernBackStepAction>(this));
				}
				attackNode->AddNode("Breath", 1, SelectRule::Non, nullptr, std::make_shared<WyvernBreathAction>(this));
				attackNode->AddNode("JumpOn", 1, SelectRule::Non, nullptr, std::make_shared<WyvernJumpOnAction>(this));
			}
			auto pursuitNode = battleNode->AddNode("Pursuit", 2, SelectRule::Non, nullptr, std::make_shared<WyvernPursuitAction>(this));
		}
		auto scoutNode = rootNode->AddNode("Scout", 5, SelectRule::Priority, nullptr, nullptr);
		{
			//auto wanderNode = scoutNode->AddNode("Wander", 1, SelectRule::Random, std::make_shared<WyvernWanderJudgment>(this), nullptr);
			//{
			//	wanderNode->AddNode("Normal", 1, SelectRule::Non, nullptr, std::make_shared<WyvernNormalAction>(this));
			//}
			//auto idleNode = scoutNode->AddNode("Idle", 2, SelectRule::Random, std::make_shared<WyvernIdleJudgment>(this), nullptr);
			//{
			//	idleNode->AddNode("Normal", 1, SelectRule::Non, nullptr, std::make_shared<WyvernNormalAction>(this));
			//}
			scoutNode->AddNode("Idle", 2, SelectRule::Non, nullptr, std::make_shared<WyvernIdleAction>(this));
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
	if (_activeNode != nullptr)
		ImGui::Text("ActiveNode : %s", _activeNode->GetName().c_str());
	// ビヘイビアツリーのGUI描画
	DrawBehaviorTreeGui(_behaviorTree->GetRoot().get());
}

// ビヘイビアツリーのGUI描画
void WyvernBehaviorTree::DrawBehaviorTreeGui(BehaviorNodeBase<WyvernBehaviorTree>* node)
{
	if (node == nullptr)
		return;

	if (ImGui::TreeNode(node->GetName().c_str()))
	{
		ImGui::Text(u8"選択ルール: %s", nameof::nameof_enum(node->GetSelectRule()).data());
		ImGui::Text(u8"判定処理: %s", node->GetJudgment() ? u8"あり" : u8"なし");
		ImGui::Text(u8"実行処理: %s", node->GetAction() ? u8"あり" : u8"なし");
		auto parent = node->GetParent();
		ImGui::Text(u8"親: %s", parent != nullptr ? parent->GetName().c_str() : u8"なし");
		auto sibling = node->GetSibling();
		ImGui::Text(u8"兄弟: %s", sibling != nullptr ? sibling->GetName().c_str() : u8"なし");
		// 子どものGUI描画
		for (int i = 0;; i++)
		{
			auto child = node->GetChild(i);
			if (child == nullptr)
				break;
			DrawBehaviorTreeGui(node->GetChild(i).get());
		}
		ImGui::TreePop();
	}
}
