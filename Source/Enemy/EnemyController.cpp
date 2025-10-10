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
	_effectController = GetActor()->GetComponent<EffectController>();
	_damageable = GetActor()->GetComponent<Damageable>();

	if (_damageable.lock())
	{
		// ダメージを受けた時の処理
		_damageable.lock()->SetOnDamageCallback(
			[&](float damage, Vector3 hitPosition) -> void
			{
				_damageCounter += damage;
				if (_damageCounter >= _damageReactionRate)
				{
					// ダメージリアクションを行う
					SetPerformDamageReaction(true);
					_damageCounter = 0.0f;
				}
			}
		);
	}
}
// 更新処理
void EnemyController::Update(float elapsedTime)
{
	// 攻撃クールタイム処理
	if (_attackCooldown > 0.0f)
	{
		// 攻撃クールタイムを減少
		_attackCooldown -= elapsedTime;
		if (_attackCooldown < 0.0f)
			_attackCooldown = 0.0f;
	}
}
// 3D描画後の描画処理
void EnemyController::DelayedRender(const RenderContext& rc)
{
	// 頭上にHPパーセント表示
	//std::wstring str = L"HP:";
	//str += std::to_wstring(static_cast<int>(_damageable.lock()->GetHealth() / _damageable.lock()->GetMaxHealth() * 100.0f));
	//str += L"%";
	//GetActor()->GetScene()->GetTextRenderer().Draw3D(
	//	FontType::MSGothic,
	//	str.c_str(),
	//	GetActor()->GetTransform().GetPosition() + Vector3(0.0f, 4.0f, 0.0f),
	//	Vector4::White
	//);
}
// GUI描画
void EnemyController::DrawGui()
{
	ImGui::Text(u8"体力 : %f", _damageable.lock()->GetHealth());
	ImGui::DragFloat(u8"ダメージリアクションの間隔", &_damageReactionRate, 0.1f, 0.0f, 10.0f, "%.1f s");
	ImGui::Text(u8"ダメージカウンター: %.1f", _damageCounter);
	ImGui::Separator();

	ImGui::DragFloat3(u8"ターゲット位置", &_targetPosition.x, 0.1f, -100.0f, 100.0f, "%.1f m");
	ImGui::DragFloat(u8"ターゲット半径", &_targetRadius, 0.1f, 0.0f, 100.0f, "%.1f m");
	ImGui::DragFloat(u8"攻撃力", &_ATK, 0.1f, 0.0f, 100.0f, "%.1f");
	ImGui::DragFloat(u8"探索範囲", &_searchRange, 0.1f, 0.0f, 100.0f, "%.1f m");
	ImGui::DragFloat(u8"攻撃範囲", &_attackRange, 0.1f, 0.0f, 100.0f, "%.1f m");
	ImGui::DragFloat(u8"近接攻撃範囲", &_nearAttackRange, 0.1f, 0.0f, 100.0f, "%.1f m");
	ImGui::DragFloat(u8"回転速度", &_rotationSpeed, 0.1f, 0.0f, 10.0f, "%.1f");
	ImGui::DragFloat(u8"ターゲットに向く角度", &_lookAtRadian, 0.01f, 0.0f, DirectX::XM_PI, "%.1f rad");
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
				if (_effectController.lock())
				{
					_effectController.lock()->Play(0, collisionData.hitPosition);
				}
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
// ターゲット方向に移動
void EnemyController::MoveToTarget(const Vector3& targetPosition, float speed)
{
	auto& position = GetActor()->GetTransform().GetPosition();
	auto targetDirection = (targetPosition - position).Normalize();
	// ターゲット方向に移動
	auto charactorController = GetCharactorController();
	Vector2 targetDirection2D = Vector2(targetDirection.x, targetDirection.z);
	charactorController->SetVelocity(
		Vector3(
			targetDirection2D.x * speed,
			charactorController->GetVelocity().y, // y軸の速度はそのまま
			targetDirection2D.y * speed
		)
	);
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
// ターゲットの範囲内かどうか
bool EnemyController::IsInTargetRange(const Vector3& targetPosition, float targetRadius)
{
	return (targetPosition - GetActor()->GetTransform().GetPosition()).Length() <= targetRadius;
}
