#include "WeakBehaviorTree.h"

#include "../../Library/Component/Animator.h"
#include "../../Library/Scene/Scene.h"

#include "../../Source/Enemy/EnemyController.h"
#include "WeakActionDerived.h"
#include "WeakJudgmentDerived.h"

#include "../../External/nameof/include/nameof.hpp"
#include "../../External/magic_enum/include/magic_enum/magic_enum.hpp"
#include <imgui.h>

WeakBehaviorTree::WeakBehaviorTree(WeakStateMachine* stateMachine, Animator* animator, MetaAI* metaAI) :
	_stateMachine(stateMachine),
	_animator(animator),
	_metaAI(metaAI)
{
	_behaviorData = std::make_unique<BehaviorData<WeakBehaviorTree>>();
	_behaviorTree = std::make_unique<BehaviorTreeBase<WeakBehaviorTree>>(this);

	// ビヘイビアツリーを構築
	auto rootNode = _behaviorTree->GetRoot();
	{
		auto fightingNode = rootNode->AddNode("Fighting", 0, SelectRule::Priority, std::make_shared<WeakFightingJudgment>(this), nullptr);
		{
			auto battleNode = fightingNode->AddNode("Battle", 0, SelectRule::Priority, std::make_shared<WeakBattleJudgment>(this), nullptr);
			{
				battleNode->AddNode("NearAttack", 0, SelectRule::Non, std::make_shared<WeakNearAttackJudgment>(this), std::make_shared<WeakCompleteStateAction>(this, "Attack"));

				auto jumpAttackNode = battleNode->AddNode("JumpAttack", 1, SelectRule::Random, std::make_shared<WeakNearJumpAttackJudgment>(this), nullptr);
				{
					jumpAttackNode->AddNode("JumpAttackPursuit", 1, SelectRule::Non, nullptr, std::make_shared<WeakCompleteStatePursuitAction>(this, "Dash"));
					jumpAttackNode->AddNode("JumpAttack", 2, SelectRule::Non, nullptr, std::make_shared<WeakCompleteStateAction>(this, "JumpAttack"));
				}

				battleNode->AddNode("Pursuit", 2, SelectRule::Non, nullptr, std::make_shared<WeakCompleteStatePursuitAction>(this, "Dash"));
			}

			fightingNode->AddNode("Threat", 1, SelectRule::Non, nullptr, std::make_shared<WeakCompleteStateAction>(this, "Threat"));
		}
		auto scoutNode = rootNode->AddNode("Scout", 1, SelectRule::Random, nullptr, nullptr);
		{
			scoutNode->AddNode("Wander", 1, SelectRule::Non, nullptr, std::make_shared<WeakSearchTargetAction>(this, "Move"));
			scoutNode->AddNode("Rest", 1, SelectRule::Non, nullptr, std::make_shared<WeakCompleteStateAction>(this, "Rest"));
		}
	}
}
// 開始処理
void WeakBehaviorTree::Start()
{
}
// ビヘイビアツリー実行
void WeakBehaviorTree::Execute(float elapsedTime)
{
	// 現在の実行ノードがなければ取得
	if (_activeNode == nullptr)
	{
		auto enemy = GetStateMachine()->GetEnemy();
		auto position = enemy->GetActor()->GetTransform().GetWorldPosition();
		float searchRange = enemy->GetSearchRange();

		// メタAIからターゲット座標を取得
		auto targetable = _metaAI->SearchTarget(
			Targetable::Faction::Player,
			position,
			searchRange);
		if (targetable)
		{
			enemy->SetTargetPosition(targetable->GetActor()->GetTransform().GetWorldPosition());
			enemy->SetTargetRadius(enemy->GetAttackRange());
			enemy->SetInFighting(true);
		}
		else
		{
			// 戦闘状態を解除
			enemy->SetInFighting(false);
			// メタAIにランダムな位置を取得してもらう
			enemy->SetTargetPosition(_metaAI->GetRandomPositionInRange(position, 100.0f));
		}

		// 推論
		_activeNode = _behaviorTree->ActiveNodeInference(_behaviorData.get());
	}
	// 実行ノードがあれば実行
	if (_activeNode != nullptr)
	{
		_activeNode = _behaviorTree->Execute(_activeNode, _behaviorData.get(), elapsedTime);
	}
}
// GUI描画
void WeakBehaviorTree::DrawGui()
{
	if (_activeNode != nullptr)
		ImGui::Text("ActiveNode : %s", _activeNode->GetName().c_str());
	// ビヘイビアツリーのGUI描画
	DrawBehaviorTreeGui(_behaviorTree->GetRoot().get());
}
// ビヘイビアツリーのGUI描画
void WeakBehaviorTree::DrawBehaviorTreeGui(BehaviorNodeBase<WeakBehaviorTree>* node)
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
