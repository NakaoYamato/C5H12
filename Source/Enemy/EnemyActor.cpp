#include "EnemyActor.h"

#include "../../Library/Scene/Scene.h"

#include "EnemyController.h"
#include "../../Library/Component/StateController.h"
#include "../../Source/Network/NetworkReceiver.h"
#include "../../Source/Common/DamageSender.h"
#include "EnemyNetworkSender.h"
#include "../../Source/AI/MetaAI.h"

void EnemyActor::OnCreate()
{
	_charactorController = this->AddComponent<CharactorController>();
	_damageable = this->AddComponent<Damageable>();
	auto damageSender = this->AddComponent<DamageSender>();
	_targetable = this->AddComponent<Targetable>();
	_combatStatus = this->AddComponent<CombatStatusController>();
	auto roarController = this->AddComponent<RoarController>();
	_staminaController = this->AddComponent<StaminaController>();

	auto networkReceiver = this->AddComponent<NetworkReceiver>();
	auto networkSender = this->AddComponent<EnemyNetworkSender>();
	// ネットワーク受信イベントの設定
	if (networkReceiver)
	{
		networkReceiver->GetEventBus().Subscribe<Network::CharacterSync>([this](const Network::CharacterSync& sync)
			{
				this->GetTransform().SetPosition(sync.position);
				this->GetTransform().SetAngleY(sync.angleY);
				auto damageable = this->GetComponent<Damageable>();
				if (damageable)
				{
					damageable->ResetHealth(sync.health);
				}
			});
		networkReceiver->GetEventBus().Subscribe<Network::CharacterMove>([this](const Network::CharacterMove& move)
			{
				auto networkReceiver = this->AddComponent<NetworkReceiver>();
				// クライアントが管理している敵ならスキップ
				if (move.senderID == networkReceiver->GetManagerId())
					return;
				this->GetTransform().SetPosition(move.position);
				this->GetTransform().SetAngleY(move.angleY);
				auto combatStatus = this->GetComponent<CombatStatusController>();
				if (combatStatus)
				{
					combatStatus->SetTargetPosition(move.target);
				}
				//auto stateController = this->GetComponent<StateController>();
				//if (stateController)
				//{
				//	// 敵の状態を更新
				//	stateController->ChangeState(move.mainState, move.subState);
				//}
			});
		networkReceiver->GetEventBus().Subscribe<Network::CharacterApplyDamage>([this](const Network::CharacterApplyDamage& applyDamage)
			{
				auto networkReceiver = this->AddComponent<NetworkReceiver>();
				// クライアントが管理している敵ならスキップ
				if (applyDamage.senderID == networkReceiver->GetManagerId())
					return;
				auto damageable = this->GetComponent<Damageable>();
				if (damageable)
				{
					damageable->AddDamage(applyDamage.damage, applyDamage.hitPosition, true);
				}
			});
	}

#pragma region コールバック関数設定
	_damageable.lock()->RegisterOnDeathCallback("EnemyActor", [&]()
		{
			// メタAIに倒された敵の名前を登録
			if (auto gameManager = GetScene()->GetActorManager().FindByName("GameManager"))
			{
				auto metaAI = gameManager->GetComponent<MetaAI>();
				if (metaAI)
				{
					metaAI->RegisterDefeatedEnemyName(this->GetName());
				}
			}
		});
#pragma endregion


	damageSender->SetHitEffectIndex(0/*TODO : enum化*/);
	_targetable.lock()->SetFaction(Targetable::Faction::Enemy);
}
