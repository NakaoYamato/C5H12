#include "WyvernEnemyController.h"

#include "../../Library/Scene/Scene.h"
#include "../../Source/Player/PlayerController.h"

#include <imgui.h>

// 開始処理
void WyvernEnemyController::Start()
{
	EnemyController::Start();

	_charactorController.lock()->SetRadius(2.5f);
	// ビヘイビアツリー作成
	_behaviorTree = std::make_unique<WyvernBehaviorTree>(this, _animator.lock().get());
	// ステートマシン作成
	_stateMachine = std::make_unique<WyvernStateMachine>(this, _animator.lock().get());
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

	// ビヘイビアツリーの実行
	_behaviorTree->Execute(elapsedTime);
	// ステートマシンの実行
	_stateMachine->Execute(elapsedTime);
}
// GUI描画
void WyvernEnemyController::DrawGui()
{
	EnemyController::DrawGui();
	_behaviorTree->DrawGui();
	_stateMachine->DrawGui();
}
// ダメージを与える
void WyvernEnemyController::AddDamage(float damage, Vector3 hitPosition)
{
	EnemyController::AddDamage(damage, hitPosition);
	_damageCounter += damage;
}
