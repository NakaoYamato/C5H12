#include "WyvernBehaviorTree.h"

#include "../../Library/Component/Animator.h"
#include "../../Library/Scene/Scene.h"

#include "../WyvernController.h"
#include "WyvernActionDerived.h"
#include "WyvernJudgmentDerived.h"

#include "../../External/nameof/include/nameof.hpp"
#include "../../External/magic_enum/include/magic_enum/magic_enum.hpp"
#include <imgui.h>

WyvernBehaviorTree::WyvernBehaviorTree(WyvernStateMachine* stateMachine, Actor* owner) :
	_stateMachine(stateMachine)
{
	// オーナーからコンポーネントを取得
	_animator	= owner->GetComponent<Animator>().get();
	_combatStatus = owner->GetComponent<CombatStatusController>().get();

	_behaviorData = std::make_unique<BehaviorData<WyvernBehaviorTree>>();
	_behaviorTree = std::make_unique<BehaviorTreeBase<WyvernBehaviorTree>>(this);

	// ビヘイビアツリーを構築
	auto rootNode = _behaviorTree->GetRoot();
	{
		// 警戒
		auto alertNode = rootNode->AddNode("Alert", 4, SelectRule::Priority, std::make_shared<WyvernAlertJudgment>(this), nullptr);
		{
			alertNode->AddNode("AlertToTarget", 0, SelectRule::Non, nullptr, std::make_shared<WyvernCompleteStateAction>(this, "ToTarget"));
			alertNode->AddNode("AlertTurn", 1, SelectRule::Non, std::make_shared<WyvernTurnJudgment>(this), std::make_shared<WyvernCompleteStateAction>(this, "Turn"));
			alertNode->AddNode("AlertThreat", 2, SelectRule::Non, std::make_shared<WyvernNearAttackJudgment>(this), std::make_shared<WyvernCompleteStateAction>(this, "Threat"));
			alertNode->AddNode("AlertRoar", 3, SelectRule::Non, std::make_shared<WyvernRoarJudgment>(this), std::make_shared<WyvernRoarAction>(this, "Roar"));
		}

		// 怒り移行
		auto angryNode = rootNode->AddNode("Angry", 3, SelectRule::Sequence, std::make_shared<WyvernAngryJudgment>(this), nullptr);
		{
			angryNode->AddNode("AngryRoar", 1, SelectRule::Non, nullptr, std::make_shared<WyvernCompleteStateAction>(this, "Roar"));
			angryNode->AddNode("AngryBackJumpBall", 2, SelectRule::Non, nullptr, std::make_shared<WyvernCompleteStateAction>(this, "BackJumpBallAttack", "HoverIdle"));
		}
		
		// 滞空
		auto hoverNode = rootNode->AddNode("Hover", 2, SelectRule::Priority, std::make_shared<WyvernFlightJudgment>(this), nullptr);
		{
			hoverNode->AddNode("HoverEnd", 4, SelectRule::Non, std::make_shared<WyvernHoverEndJudgment>(this), std::make_shared<WyvernCompleteStateAction>(this, "Land"));
			
			auto nearNode = hoverNode->AddNode("HoverNear", 3, SelectRule::NoDuplicatesRandom, std::make_shared<WyvernHoverNearJudgment>(this), nullptr);
			{
				nearNode->AddNode("HoverClaw", 1, SelectRule::Non, nullptr, std::make_shared<WyvernCompleteStateAction>(this, "HoverClawAttack", "HoverIdle"));
				nearNode->AddNode("HoverTurn", 1, SelectRule::Non, std::make_shared<WyvernTurnJudgment>(this), std::make_shared<WyvernCompleteStateAction>(this, "HoverTurn", "HoverIdle"));
			}

			auto farNode = hoverNode->AddNode("HoverBall", 2, SelectRule::Priority, std::make_shared<WyvernAttackJudgment>(this), std::make_shared<WyvernCompleteStateAction>(this, "HoverFireBallAttack", "HoverIdle"));

			hoverNode->AddNode("HoverToTarget", 1, SelectRule::Non, nullptr, std::make_shared<WyvernCompleteStateAction>(this, "HoverToTarget", "HoverIdle"));
			hoverNode->AddNode("HoverIdle", 0, SelectRule::Non, nullptr, std::make_shared<WyvernTimerAction>(this, "HoverIdle", 2.0f));
		}

		// 地上戦闘
		auto battleNode = rootNode->AddNode("Battle", 1, SelectRule::Priority, std::make_shared<WyvernBattleJudgment>(this), nullptr);
		{
			auto confrontNode = battleNode->AddNode("Confront", 2, SelectRule::Priority, std::make_shared<WyvernConfrontJudgment>(this), nullptr);
			{
				confrontNode->AddNode("ToTarget", 0, SelectRule::Non, nullptr, std::make_shared<WyvernCompleteStateAction>(this, "ToTarget"));
				confrontNode->AddNode("Turn", 1, SelectRule::Non, std::make_shared<WyvernTurnJudgment>(this), std::make_shared<WyvernCompleteStateAction>(this, "Turn"));
			}

			auto attackNode = battleNode->AddNode("Attack", 1, SelectRule::Priority, std::make_shared<WyvernAttackJudgment>(this), nullptr);
			{
				auto nearAttack = attackNode->AddNode("NearAttack", 1, SelectRule::NoDuplicatesRandom, std::make_shared<WyvernNearAttackJudgment>(this), nullptr);
				{
					nearAttack->AddNode("Bite", 4, SelectRule::Non, nullptr, std::make_shared<WyvernCompleteStateAction>(this, "BiteAttack"));
					nearAttack->AddNode("Claw", 4, SelectRule::Non, nullptr, std::make_shared<WyvernCompleteStateAction>(this, "ClawAttack"));
					nearAttack->AddNode("Tail", 2, SelectRule::Non, nullptr, std::make_shared<WyvernCompleteStateAction>(this, "TailAttack"));
					nearAttack->AddNode("BackStep", 1, SelectRule::Non, nullptr, std::make_shared<WyvernCompleteStateAction>(this, "BackStep"));
				}
				auto farAttack = attackNode->AddNode("FarAttack", 0, SelectRule::NoDuplicatesRandom, nullptr, nullptr);
				{
					farAttack->AddNode("Ball", 2, SelectRule::Non, nullptr, std::make_shared<WyvernCompleteStateAction>(this, "BallAttack"));
					auto chargeNode = farAttack->AddNode("Charge", 2, SelectRule::Sequence, nullptr, nullptr);
					{
						chargeNode->AddNode("Alignment", 1, SelectRule::Non, nullptr, std::make_shared<WyvernTimerAction>(this, "BackStep", GetStateMachine()->GetWyvern()->GetChargeAttackChargeTime()));
						chargeNode->AddNode("ChargeAttack", 2, SelectRule::Non, nullptr, std::make_shared<WyvernCompleteStateAction>(this, "ChargeAttack"));
					}
				}
			}
			auto pursuitNode = battleNode->AddNode("Pursuit", 0, SelectRule::Non, nullptr, std::make_shared<WyvernCompleteStateAction>(this, "Pursuit"));
		}

		// 偵察
		auto scoutNode = rootNode->AddNode("Scout", 0, SelectRule::Priority, nullptr, nullptr);
		{
			//auto wanderNode = scoutNode->AddNode("Wander", 1, SelectRule::Random, std::make_shared<WyvernWanderJudgment>(this), nullptr);
			//{
			//	wanderNode->AddNode("Normal", 1, SelectRule::Non, nullptr, std::make_shared<WyvernNormalAction>(this));
			//}
			scoutNode->AddNode("Idle", 2, SelectRule::Non, nullptr, nullptr);
		}
	}
}
// 開始処理
void WyvernBehaviorTree::Start()
{
}
// ビヘイビアツリー実行
void WyvernBehaviorTree::Execute(float elapsedTime)
{
	// 現在の実行ノードがなければ取得
	if (_activeNode == nullptr)
	{
		// ターゲットを更新する
		GetCombatStatus()->SetIsUpdateTarget(true);
		GetCombatStatus()->Update(0.0f);
		// 推論
		_activeNode = _behaviorTree->ActiveNodeInference(_behaviorData.get());
		// ターゲットを更新しないようにする
		GetCombatStatus()->SetIsUpdateTarget(false);
	}
	// 実行ノードがあれば実行
	if (_activeNode != nullptr)
	{
		_activeNode = _behaviorTree->Execute(_activeNode, _behaviorData.get(), elapsedTime);
	}
}
// GUI描画
void WyvernBehaviorTree::DrawGui()
{
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
