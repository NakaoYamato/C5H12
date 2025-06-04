#include "WyvernBreathController.h"

#include "../../Library/Scene/Scene.h"
#include "../../Library/Math/Random.h"
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

	// パーティクル生成
	Vector3 pos = GetActor()->GetTransform().GetPosition();
	for (int i = 0; i < _particleCount; i++)
	{
		Vector3 p = pos;
		p.x += 0.5f * Random::RandBias();
		p.y += 0.5f * Random::RandBias();
		p.z += 0.5f * Random::RandBias();

		// TODO : 放射状に飛ばす
		Vector3 v = _breathDirection * _particleSpeed;
		v.x *= _particleSpread.x * Random::RandNormal();
		v.y *= _particleSpread.y * Random::RandNormal();
		v.z *= _particleSpread.z * Random::RandNormal();

		DirectX::XMFLOAT3 f = _breathDirection;
		f.x *= 2.0f * Random::RandNormal();
		f.y *= 2.0f * Random::RandNormal();
		f.z *= 2.0f * Random::RandNormal();
		DirectX::XMFLOAT2 s = { _particleScale,_particleScale };

		ParticleRenderer::EmitData data{};
		// 更新タイプ
		data.type = ParticleRenderer::ParticleType::Billboard;
		data.texcoordIndex = 2;
		data.timer = _particleLifeTime;
		data.texAnimTime = 0.4f;
		// 発生位置
		data.position = p;
		// 発生方向
		data.velocity = v;
		// 加速力
		data.acceleration.x = f.x;
		data.acceleration.y = f.y;
		data.acceleration.z = f.z;
		// 大きさ
		data.scale.x = s.x;
		data.scale.y = s.y;
		data.scale.z = 0.0f;

		GetActor()->GetScene()->GetParticleRenderer().Emit(data);
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
