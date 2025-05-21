#include "WyvernEnemyController.h"

#include "../../Library/Scene/Scene.h"

#include <imgui.h>

// 開始処理
void WyvernEnemyController::Start()
{
	EnemyController::Start();
	// ステートマシン初期化
	_stateMachine = std::make_unique<WyvernStateMachine>(this, _animator.lock().get());

	_behaviorTree = std::make_unique<WyvernBehaviorTree>(this, _animator.lock().get());
}
// 更新処理
void WyvernEnemyController::Update(float elapsedTime)
{
	// ターゲット座標にプレイヤー座標を設定
	{
		auto& actorManager = GetActor()->GetScene()->GetActorManager();
		auto& playerTags = actorManager.FindByTag(ActorTag::Player);
		auto& player = playerTags[0];
		if (player != nullptr)
		{
			SetTargetPosition(player->GetTransform().GetPosition());
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
