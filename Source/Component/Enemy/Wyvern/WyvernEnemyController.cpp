#include "WyvernEnemyController.h"

#include "../../Library/Scene/Scene.h"
#include "../../Source/Component/Player/PlayerController.h"

#include <imgui.h>

// 開始処理
void WyvernEnemyController::Start()
{
	EnemyController::Start();

	_charactorController.lock()->SetRadius(2.5f);
	// ビヘイビアツリー作成
	_behaviorTree = std::make_unique<WyvernBehaviorTree>(this, _animator.lock().get());
}
// 更新処理
void WyvernEnemyController::Update(float elapsedTime)
{
	// ターゲット座標にプレイヤー座標を設定
	{
		auto& actorManager = GetActor()->GetScene()->GetActorManager();
		auto& playerTags = actorManager.FindByTag(ActorTag::Player);
		for (auto& playerTag : playerTags)
		{
			// playerControllerを持っているアクターを取得
			auto playerController = playerTag->GetComponent<PlayerController>();
			if (playerController == nullptr)
				continue;
			SetTargetPosition(playerTag->GetTransform().GetPosition());

			break;
		}
	}

	_behaviorTree->Run(elapsedTime);
}
// GUI描画
void WyvernEnemyController::DrawGui()
{
	_behaviorTree->DrawGui();
}
// ダメージを与える
void WyvernEnemyController::AddDamage(float damage, Vector3 hitPosition)
{
	EnemyController::AddDamage(damage, hitPosition);
}
