#include "WyvernBehaviorTree.h"

#include "../../Library/Component/Animator.h"
#include "../../Library/Scene/Scene.h"

#include "../WyvernController.h"
#include "WyvernActionDerived.h"
#include "WyvernJudgmentDerived.h"

#include "../../External/nameof/include/nameof.hpp"
#include "../../External/magic_enum/include/magic_enum/magic_enum.hpp"
#include <imgui.h>

typedef BehaviorCallbackJudgment<WyvernBehaviorTree> WyvernCallbackJudgment;

WyvernBehaviorTree::WyvernBehaviorTree(WyvernStateMachine* stateMachine, Actor* owner) :
	_stateMachine(stateMachine),
	_owner(owner)
{
	_behaviorData = std::make_unique<BehaviorData<WyvernBehaviorTree>>();
	_behaviorTree = std::make_unique<BehaviorTreeBase<WyvernBehaviorTree>>(this);

	// ビヘイビアツリーを構築
	auto rootNode = _behaviorTree->GetRoot();
	{
		// ダウン処理
		auto downNode = rootNode->AddNode("Down", 6, SelectRule::Priority, 
			std::make_shared<WyvernCallbackJudgment>(this, [&](){ return _interruptionName == "Down"; }), nullptr);
		{
			downNode->AddNode("FallDown", 1, SelectRule::Non, 
				std::make_shared<WyvernFlightJudgment>(this), std::make_shared<WyvernCompleteStateAction>(this, "HitFall"));
			auto landDownNode = downNode->AddNode("LandDown", 0, SelectRule::Priority, nullptr, nullptr);
			{
				landDownNode->AddNode("DownLeft", 1, SelectRule::Non,
					std::make_shared<WyvernCallbackJudgment>(this, [&]() { return _bodyPartName == "LeftWing" || _bodyPartName == "LeftFoot"; }),
					std::make_shared<WyvernCompleteSubStateAction>(this, "Down", u8"DownLStart"));
				landDownNode->AddNode("DownRight", 0, SelectRule::Non,
					std::make_shared<WyvernCallbackJudgment>(this, [&]() { return _bodyPartName == "RightWing" || _bodyPartName == "RightFoot"; }),
					std::make_shared<WyvernCompleteSubStateAction>(this, "Down", u8"DownRStart"));
			}
		}

		// ダメージ処理
		auto damageNode = rootNode->AddNode("Damage", 5, SelectRule::Priority,
			std::make_shared<WyvernCallbackJudgment>(this, [&]() { return _interruptionName == "Damage"; }), nullptr);
		{
			damageNode->AddNode("FallDamage", 1, SelectRule::Non, 
				std::make_shared<WyvernFlightJudgment>(this), std::make_shared<WyvernCompleteStateAction>(this, "HitFall"));
			auto landDamageNode = damageNode->AddNode("LandDamage", 0, SelectRule::Priority, nullptr, nullptr);
			{
				landDamageNode->AddNode("DamageHead", 6, SelectRule::Non,
					std::make_shared<WyvernCallbackJudgment>(this, [&]() { return _bodyPartName == "Head"; }),
					std::make_shared<WyvernCompleteSubStateAction>(this, "Damage", u8"DamageLeft"));
				landDamageNode->AddNode("DamageBody", 5, SelectRule::Non,
					std::make_shared<WyvernCallbackJudgment>(this, [&]() { return _bodyPartName == "Body"; }),
					std::make_shared<WyvernCompleteSubStateAction>(this, "Damage", u8"DamageLeft"));
				landDamageNode->AddNode("DamageLeft", 4, SelectRule::Non,
					std::make_shared<WyvernCallbackJudgment>(this, [&]() { return _bodyPartName == "LeftWing"; }),
					std::make_shared<WyvernCompleteSubStateAction>(this, "Damage", u8"DamageLeft"));
				landDamageNode->AddNode("DamageBackLeft", 3, SelectRule::Non,
					std::make_shared<WyvernCallbackJudgment>(this, [&]() { return _bodyPartName == "LeftFoot"; }),
					std::make_shared<WyvernCompleteSubStateAction>(this, "Damage", u8"DamageBackLeft"));
				landDamageNode->AddNode("DamageRight", 2, SelectRule::Non,
					std::make_shared<WyvernCallbackJudgment>(this, [&]() { return _bodyPartName == "RightWing"; }),
					std::make_shared<WyvernCompleteSubStateAction>(this, "Damage", u8"DamageRight"));
				landDamageNode->AddNode("DamageBackRight", 1, SelectRule::Non,
					std::make_shared<WyvernCallbackJudgment>(this, [&]() { return _bodyPartName == "RightFoot"; }),
					std::make_shared<WyvernCompleteSubStateAction>(this, "Damage", u8"DamageBackRight"));
				landDamageNode->AddNode("DamageTail", 0, SelectRule::Non,
					std::make_shared<WyvernCallbackJudgment>(this, [&]() { return _bodyPartName == "Tail"; }),
					std::make_shared<WyvernCompleteSubStateAction>(this, "Damage", u8"DamageBackRight"));
			}
		}

		// 怒り移行
		auto angryNode = rootNode->AddNode("Angry", 4, SelectRule::Sequence, std::make_shared<WyvernAngryJudgment>(this), nullptr);
		{
			angryNode->AddNode("AngryRoar", 1, SelectRule::Non, nullptr,
				std::make_shared<WyvernCompleteStateAction>(this, "Roar"));
			angryNode->AddNode("AngryBackJumpBall", 2, SelectRule::Non, nullptr, 
				std::make_shared<WyvernAttackAction>(this, "BackJumpBallAttack", nullptr, 0.0f));
		}
		
		// 滞空
		auto hoverNode = rootNode->AddNode("Hover", 3, SelectRule::Priority, std::make_shared<WyvernFlightJudgment>(this), nullptr);
		{
			hoverNode->AddNode("HoverEnd", 4, SelectRule::Non,
				std::make_shared<WyvernHoverEndJudgment>(this), std::make_shared<WyvernCompleteStateAction>(this, "Land"));
			
			auto nearNode = hoverNode->AddNode("HoverNear", 3, SelectRule::NoDuplicatesRandom,
				std::make_shared<WyvernHoverNearJudgment>(this), nullptr);
			{
				//nearNode->AddNode("HoverClaw", 1, SelectRule::Non,
				//	nullptr, std::make_shared<WyvernAttackAction>(this, "HoverClawAttack", nullptr, AttackStaminaCost));
				nearNode->AddNode("HoverTurn", 1, SelectRule::Non, 
					std::make_shared<WyvernTurnJudgment>(this), std::make_shared<WyvernCompleteStateAction>(this, "HoverTurn", MoveStaminaCost));
			}

			auto farNode = hoverNode->AddNode("HoverBall", 2, SelectRule::Non,
				std::make_shared<WyvernAttackJudgment>(this), std::make_shared<WyvernAttackAction>(this, "HoverFireBallAttack", nullptr, BreathAttackStaminaCost));

			hoverNode->AddNode("HoverToTarget", 1, SelectRule::Non, 
				nullptr, std::make_shared<WyvernCompleteStateAction>(this, "HoverToTarget", MoveStaminaCost));
			hoverNode->AddNode("HoverIdle", 0, SelectRule::Non,
				nullptr, std::make_shared<WyvernTimerAction>(this, "HoverIdle", 2.0f));
		}

		// 地上戦闘
		auto battleNode = rootNode->AddNode("Battle", 2, SelectRule::Priority, std::make_shared<WyvernBattleJudgment>(this), nullptr);
		{
			auto confrontNode = battleNode->AddNode("Confront", 3, SelectRule::Priority,
				std::make_shared<WyvernConfrontJudgment>(this), nullptr);
			{
				auto toTargetNode = confrontNode->AddNode("ToTarget", 0, SelectRule::Random, nullptr, nullptr);
				{
					toTargetNode->AddNode("ToTarget", 1, SelectRule::Non, nullptr, std::make_shared<WyvernCompleteStateAction>(this, "ToTarget"));
					toTargetNode->AddNode("ToBack", 1, SelectRule::Non, nullptr, std::make_shared<WyvernCompleteStateAction>(this, "BackStep"));
				}
				confrontNode->AddNode("Turn", 1, SelectRule::Non, 
					std::make_shared<WyvernTurnJudgment>(this), std::make_shared<WyvernCompleteStateAction>(this, "Turn"));
			}

			auto attackNode = battleNode->AddNode("Attack", 2, SelectRule::Priority,
				std::make_shared<WyvernAttackJudgment>(this), nullptr);
			{
				auto nearAttack = attackNode->AddNode("NearAttack", 1, SelectRule::NoDuplicatesRandom,
					std::make_shared<WyvernNearAttackJudgment>(this), nullptr);
				{
					nearAttack->AddNode("Bite", 4, SelectRule::Non, nullptr, std::make_shared<WyvernAttackAction>(this, "BiteAttack", nullptr, AttackStaminaCost));
					nearAttack->AddNode("Claw", 4, SelectRule::Non, nullptr, std::make_shared<WyvernAttackAction>(this, "ClawAttack", nullptr, AttackStaminaCost));
					nearAttack->AddNode("Tail", 2, SelectRule::Non, nullptr, std::make_shared<WyvernAttackAction>(this, "TailAttack", nullptr, AttackStaminaCost));
					nearAttack->AddNode("BackStep", 1, SelectRule::Non, nullptr, std::make_shared<WyvernCompleteStateAction>(this, "BackStep", MoveStaminaCost));
				}
				auto farAttack = attackNode->AddNode("FarAttack", 0, SelectRule::NoDuplicatesRandom, nullptr, nullptr);
				{
					farAttack->AddNode("Ball", 2, SelectRule::Non, nullptr, std::make_shared<WyvernAttackAction>(this, "BallAttack", nullptr, BreathAttackStaminaCost));
					auto chargeNode = farAttack->AddNode("Charge", 2, SelectRule::Sequence, nullptr, nullptr);
					{
						chargeNode->AddNode("Alignment", 1, SelectRule::Non, nullptr, std::make_shared<WyvernTimerAction>(this, "BackStep", GetStateMachine()->GetWyvern()->GetChargeAttackChargeTime()));
						chargeNode->AddNode("ChargeAttack", 2, SelectRule::Non, nullptr, std::make_shared<WyvernAttackAction>(this, "ChargeAttack", nullptr, AttackStaminaCost));
					}
				}
			}

			auto pursuitNode = battleNode->AddNode("Pursuit", 1, SelectRule::Non, 
				std::make_shared<WyvernStaminaJudgment>(this, 30.0f), std::make_shared<WyvernCompleteStateAction>(this, "Pursuit"));

			auto threatNode = battleNode->AddNode("BattleThreat", 0, SelectRule::Non,
				nullptr, std::make_shared<WyvernCompleteStateAction>(this, "Threat", -ThreatStaminaRecover));
		}

		// 警戒
		auto alertNode = rootNode->AddNode("Alert", 1, SelectRule::Priority, std::make_shared<WyvernAlertJudgment>(this), nullptr);
		{
			alertNode->AddNode("AlertToTarget", 0, SelectRule::Non, nullptr, std::make_shared<WyvernCompleteStateAction>(this, "ToTarget"));
			alertNode->AddNode("AlertTurn", 1, SelectRule::Non, std::make_shared<WyvernTurnJudgment>(this), std::make_shared<WyvernCompleteStateAction>(this, "Turn"));
			alertNode->AddNode("AlertThreat", 2, SelectRule::Non, std::make_shared<WyvernNearAttackJudgment>(this), std::make_shared<WyvernCompleteStateAction>(this, "Threat"));
			alertNode->AddNode("AlertRoar", 3, SelectRule::Non, std::make_shared<WyvernRoarJudgment>(this), std::make_shared<WyvernRoarAction>(this, "Roar"));
		}

		// 偵察
		auto scoutNode = rootNode->AddNode("Scout", 0, SelectRule::Priority, nullptr, nullptr);
		{
			//auto wanderNode = scoutNode->AddNode("Wander", 1, SelectRule::Random, std::make_shared<WyvernWanderJudgment>(this), nullptr);
			//{
			//	wanderNode->AddNode("Normal", 1, SelectRule::Non, nullptr, std::make_shared<WyvernNormalAction>(this));
			//}
			scoutNode->AddNode("ReturnTerritory", 1, SelectRule::Non, nullptr, std::make_shared<WyvernCompleteStateAction>(this, "ToTarget"));
			scoutNode->AddNode("Idle", 2, SelectRule::Non, std::make_shared<WyvernTargetNearJudgment>(this, 10.0f), std::make_shared<WyvernCompleteStateAction>(this, "Idle"));
		}
	}

	_behaviorTreeEditor.LoadFromFile("./Data/Debug/Behavior/Wyvern.json");
}
// 開始処理
void WyvernBehaviorTree::Start()
{
	// オーナーからコンポーネントを取得
	_animator = _owner->GetComponent<Animator>().get();
	_combatStatus = _owner->GetComponent<CombatStatusController>().get();
	_staminaController = _owner->GetComponent<StaminaController>().get();
	// 子供から部位コントローラー取得
	for (auto& child : _owner->GetChildren())
	{
		auto bodyPartController = child->GetComponent<BodyPartController>();
		if (bodyPartController)
		{
			_bodyPartControllers.push_back(bodyPartController.get());
		}
	}
}
// ビヘイビアツリー実行
void WyvernBehaviorTree::Execute(float elapsedTime)
{
	std::string currentStateName = _stateMachine->GetStateName();

	// 死亡時は処理しない
	if (currentStateName == "Death")
	{
		return;
	}

	// 被弾処理
	if (IsDamageInterruption())
	{
		// 実行ノードがあれば終了処理
		if (_activeNode != nullptr)
			_activeNode->Exit();
		// 推論
		_activeNode = _behaviorTree->ActiveNodeInference(_behaviorData.get());
		GetStateMachine()->GetEnemy()->SetPerformDamageReaction(false);
		GetStateMachine()->GetEnemy()->SetPerformDownReaction(false);
	}

	// 現在の実行ノードがなければ取得
	if (_activeNode == nullptr)
	{
		// 割り込み処理名初期化
		_interruptionName = "";
		_bodyPartName = "";

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
    _behaviorTreeEditor.Draw(_behaviorTree.get(), _activeNode);
}

// 被弾割り込み処理が発生しているか
bool WyvernBehaviorTree::IsDamageInterruption()
{
	std::string prevInterruptionName = _interruptionName;

	bool res = false;

	// 各部位の判定を取得
	for (auto bodyPartController : _bodyPartControllers)
	{
		if (bodyPartController->IsDown())
		{
			res = true;
			_interruptionName = "Down";
			_bodyPartName = bodyPartController->GetBodyPartName();
		}

		if (_interruptionName == "" && bodyPartController->IsStagger())
		{
			res = true;
			_interruptionName = "Damage";
			_bodyPartName = bodyPartController->GetBodyPartName();
		}
		bodyPartController->SetIsStagger(false);
		bodyPartController->SetIsDown(false);
	}
	
	// ダウン中は処理しない
	if (prevInterruptionName == "Down")
		return false;

	return res;
}
