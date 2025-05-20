#include "EnemyController.h"

#include "../../Source/Component/Player/PlayerController.h"

#include <imgui.h>

void EnemyController::Start()
{
	// コンポーネント取得
	_charactorController = GetActor()->GetComponent<CharactorController>();
	_animator = GetActor()->GetComponent<Animator>();
}

void EnemyController::Update(float elapsedTime)
{
}

void EnemyController::DrawGui()
{
	ImGui::Text(u8"体力 : %f", _health);
}

void EnemyController::OnContact(CollisionData& collisionData)
{
	// 攻撃判定
	if (collisionData.myLayer == CollisionLayer::Attack)
	{
		auto player = collisionData.other->GetComponent<PlayerController>();
		if (player != nullptr)
		{
			// プレイヤーにダメージを与える
			player->AddDamage(_ATK, _hitPosition);
		}
	}
}

void EnemyController::AddDamage(float damage, Vector3 hitPosition)
{
	_health -= damage;
	_hitPosition = hitPosition;
}
