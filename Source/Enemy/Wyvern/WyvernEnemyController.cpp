#include "WyvernEnemyController.h"

#include "../../Library/Scene/Scene.h"
#include "../../Source/Player/PlayerController.h"

#include <imgui.h>

// 開始処理
void WyvernEnemyController::Start()
{
	EnemyController::Start();

	// 体力設定
	_maxHealth = 10.0f;
	_health = _maxHealth;

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

// ダメージを受けた時の処理
void WyvernEnemyController::OnDamage(float damage, Vector3 hitPosition)
{
	_damageCounter += damage;
	if (_damageCounter >= _damageReactionRate)
	{
		// ダメージリアクションを行う
		SetPerformDamageReaction(true);
		_damageCounter = 0.0f;
	}
}

#pragma region ネットワーク用
void WyvernEnemyController::ChangeState(const char* mainStateName, const char* subStateName)
{
	_stateMachine->ChangeState(mainStateName, subStateName);
}

const char* WyvernEnemyController::GetStateName()
{
	return _stateMachine->GetStateName();
}

const char* WyvernEnemyController::GetSubStateName()
{
	return _stateMachine->GetSubStateName();
}
#pragma endregion
