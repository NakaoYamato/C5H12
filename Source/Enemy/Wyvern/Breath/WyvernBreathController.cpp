#include "WyvernBreathController.h"

#include "../../Source/Common/Damageable.h"

#include <imgui.h>

// 更新処理
void WyvernBreathController::Update(float elapsedTime)
{
	// ブレスの距離を更新
	_breathRange += _breathSpeed * elapsedTime;
	_breathRange = std::clamp(_breathRange, 0.0f, _breathRangeMax);
	// カプセルコライダーの位置と方向を更新
	if (auto capsuleCollider = _capsuleCollider.lock())
	{
		capsuleCollider->SetEnd(_breathDirection * _breathRange);
	}
}
// GUI描画
void WyvernBreathController::DrawGui()
{
	ImGui::DragFloat("ブレスの速度", &_breathSpeed, 1.0f, 0.0f, 1000.0f);
	ImGui::DragFloat("ブレスの距離", &_breathRange, 1.0f, 0.0f, 10000.0f);
	ImGui::DragFloat("ブレスの最大距離", &_breathRangeMax, 1.0f, 0.0f, 10000.0f);
}
// 接触時処理
void WyvernBreathController::OnContactEnter(CollisionData& collisionData)
{
	// 攻撃判定
	if (collisionData.myLayer == CollisionLayer::Attack &&
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
