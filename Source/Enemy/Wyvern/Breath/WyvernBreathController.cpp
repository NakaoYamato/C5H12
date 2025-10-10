#include "WyvernBreathController.h"

#include "../../Library/Scene/Scene.h"
#include "../../Library/Math/Random.h"
#include "../../Source/Common/Damageable.h"

#include "../../Library/DebugSupporter/DebugSupporter.h"

#include <imgui.h>

// 開始処理
void WyvernBreathController::Start()
{
	auto capsuleColliders = GetActor()->GetColliders<CapsuleCollider>();
	for (auto& capsule : capsuleColliders)
	{
		// ブレスのカプセルコライダーを登録
		_capsuleColliders.push_back(capsule);
		capsule->SetRadius(_breathRadius);
	}
	_effectController = GetActor()->GetComponent<EffectController>();
	auto& transform = GetActor()->GetTransform();
	_effectController.lock()->Play(0, transform.GetWorldPosition(), transform.GetRotation());
}

// 削除処理
void WyvernBreathController::OnDelete()
{
	_effectController.lock()->Stop(0);
}

// 更新処理
void WyvernBreathController::Update(float elapsedTime)
{
	Debug::Renderer::DrawAxis(GetActor()->GetTransform().GetMatrix());

	// ブレスの距離を更新
	_breathRange += _breathSpeed * elapsedTime;
	_breathRange = std::clamp(_breathRange, 0.0f, _breathRangeMax);
	// カプセルコライダーの位置と方向を更新
	for (size_t i = 0; i < 3; ++i)
	{
		if (i >= _capsuleColliders.size()) break;
		auto capsule = _capsuleColliders[i].lock();
		if (capsule == nullptr) continue;
		// ブレスのカプセルコライダーの位置を更新
		Vector3 direction = Vector3(Vector3::Front) + Vector3(Vector3::Right) * _breathWaver * ((float)i - 1.0f);
		capsule->SetStart(_collisionOffset);
		capsule->SetEnd(_collisionOffset + direction.Normalize() * _breathRange);
	}
}
// GUI描画
void WyvernBreathController::DrawGui()
{
	ImGui::DragFloat3(u8"ブレスのオフセット", &_collisionOffset.x, 0.1f, -100.0f, 100.0f);
	ImGui::DragFloat(u8"ブレスの速度", &_breathSpeed, 1.0f, 0.0f, 1000.0f);
	ImGui::DragFloat(u8"ブレスの距離", &_breathRange, 1.0f, 0.0f, 10000.0f);
	ImGui::DragFloat(u8"ブレスの最大距離", &_breathRangeMax, 1.0f, 0.0f, 10000.0f);
}
// 接触時処理
void WyvernBreathController::OnContactEnter(CollisionData& collisionData)
{
	// 攻撃判定
	if (collisionData.myLayer == CollisionLayer::Attack &&
		collisionData.otherLayer == CollisionLayer::Hit &&
		collisionData.other != _breathActor.lock().get())
	{
		// ダメージを与える
		auto damageable = collisionData.other->GetComponent<Damageable>();
		if (damageable != nullptr)
		{
			if (damageable->AddDamage(_ATK, collisionData.hitPosition))
			{
				// ダメージを与えたらヒットエフェクト再生
				//_hitEffectController.lock()->Play(collisionData.hitPosition, 1.0f);
			}
		}
	}
}
