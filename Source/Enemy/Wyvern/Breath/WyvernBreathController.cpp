#include "WyvernBreathController.h"

#include "../../Library/Scene/Scene.h"
#include "../../Library/Math/Random.h"
#include "../../Source/Common/Damageable.h"

#include "../../Library/DebugSupporter/DebugSupporter.h"

#include <imgui.h>

// 開始処理
void WyvernBreathController::Start()
{
	_capsuleCollider = GetActor()->GetCollider<CapsuleCollider>();
	_particleController = GetActor()->GetComponent<ParticleController>();
	_particleController.lock()->Play();

	_capsuleCollider.lock()->SetRadius(_breathRadius);
}

// 削除処理
void WyvernBreathController::Deleted()
{
	_particleController.lock()->Stop();
}

// 更新処理
void WyvernBreathController::Update(float elapsedTime)
{
	Debug::Renderer::DrawAxis(GetActor()->GetTransform().GetMatrix());

	// ブレスの距離を更新
	_breathRange += _breathSpeed * elapsedTime;
	_breathRange = std::clamp(_breathRange, 0.0f, _breathRangeMax);
	// カプセルコライダーの位置と方向を更新
	if (auto capsuleCollider = _capsuleCollider.lock())
	{
		capsuleCollider->SetEnd(Vector3(Vector3::Front) * _breathRange);
	}
}
// GUI描画
void WyvernBreathController::DrawGui()
{
	ImGui::DragFloat(u8"ブレスの速度", &_breathSpeed, 1.0f, 0.0f, 1000.0f);
	ImGui::DragFloat(u8"ブレスの距離", &_breathRange, 1.0f, 0.0f, 10000.0f);
	ImGui::DragFloat(u8"ブレスの最大距離", &_breathRangeMax, 1.0f, 0.0f, 10000.0f);
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
