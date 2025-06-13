#include "EnemyController.h"

#include "../../Source/Player/PlayerController.h"

#include "../../Library/Scene/Scene.h"
#include <imgui.h>

// 開始処理
void EnemyController::Start()
{
	// コンポーネント取得
	_charactorController = GetActor()->GetComponent<CharactorController>();
	_animator = GetActor()->GetComponent<Animator>();
	_hitEffectController = GetActor()->GetComponent<EffekseerEffectController>();
	_damageable = GetActor()->GetComponent<Damageable>();
}
// 更新処理
void EnemyController::Update(float elapsedTime)
{
}
// 3D描画後の描画処理
void EnemyController::DelayedRender(const RenderContext& rc)
{
	// 頭上にHPパーセント表示
	std::wstring str = L"HP:";
	str += std::to_wstring(static_cast<int>(_damageable.lock()->GetHealth() / _damageable.lock()->GetMaxHealth() * 100.0f));
	str += L"%";
	GetActor()->GetScene()->GetTextRenderer().Draw3D(
		FontType::MSGothic,
		str.c_str(),
		GetActor()->GetTransform().GetPosition() + Vector3(0.0f, 4.0f, 0.0f),
		Vector4::White
	);
}
// GUI描画
void EnemyController::DrawGui()
{
	ImGui::Text(u8"体力 : %f", _damageable.lock()->GetHealth());
}
// 接触時処理
void EnemyController::OnContactEnter(CollisionData& collisionData)
{
	// 攻撃判定
	if (collisionData.myLayer == CollisionLayer::Attack && collisionData.otherLayer == CollisionLayer::Hit)
	{
		// ダメージを与える
		auto damageable = collisionData.other->GetComponent<Damageable>();
		if (damageable != nullptr)
		{
			if (damageable->AddDamage(_ATK, collisionData.hitPosition))
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
