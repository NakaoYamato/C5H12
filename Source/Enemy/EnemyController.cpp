#include "EnemyController.h"

#include "../../Source/Player/PlayerController.h"

#include <imgui.h>

// 開始処理
void EnemyController::Start()
{
	// コンポーネント取得
	_charactorController = GetActor()->GetComponent<CharactorController>();
	_animator = GetActor()->GetComponent<Animator>();
	_hitEffectController = GetActor()->GetComponent<EffekseerEffectController>();
}
// 更新処理
void EnemyController::Update(float elapsedTime)
{
}
// GUI描画
void EnemyController::DrawGui()
{
	ImGui::Text(u8"体力 : %f", _health);
}
// 接触時処理
void EnemyController::OnContactEnter(CollisionData& collisionData)
{
	// 攻撃判定
	if (collisionData.myLayer == CollisionLayer::Attack)
	{
		auto player = collisionData.other->GetComponent<PlayerController>();
		if (player != nullptr)
		{
			// プレイヤーにダメージを与える
			if (player->AddDamage(_ATK, _hitPosition))
			{
				// ダメージを与えたらヒットエフェクト再生
				_hitEffectController.lock()->Play(collisionData.hitPosition, 1.0f);
			}
		}
	}
}

// 指定位置との角度
float EnemyController::GetAngleToTarget(const Vector3& target)
{
	auto& position = GetActor()->GetTransform().GetPosition();
	auto& targetPosition = GetTargetPosition();
	auto targetDirection = (targetPosition - position).Normalize();
	auto front = GetActor()->GetTransform().GetAxisZ().Normalize();
	return std::acosf(front.Dot(targetDirection));
}

// ターゲット方向を向く
void EnemyController::LookAtTarget(const Vector3& target, float elapsedTime, float rotationSpeed)
{
	auto& position = GetActor()->GetTransform().GetPosition();
	auto targetDirection = (target - position);
	// ターゲット方向に回転
	auto charactorController = GetCharactorController();
	Vector2 targetDirection2D = Vector2(targetDirection.x, targetDirection.z);
	charactorController->UpdateRotation(elapsedTime, targetDirection2D.Normalize() * rotationSpeed);
}
