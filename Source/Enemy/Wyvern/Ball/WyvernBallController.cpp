#include "WyvernBallController.h"

#include "../../Library/Scene/Scene.h"
#include "../../Library/Math/Random.h"
#include "../../Source/Common/Damageable.h"

#include "../../Library/DebugSupporter/DebugSupporter.h"

#include <imgui.h>
// 開始処理
void WyvernBallController::Start()
{
	_ballCollider = GetActor()->GetCollider<SphereCollider>();
	_effectController = GetActor()->GetComponent<EffectController>();
	auto& transform = GetActor()->GetTransform();
	_effectController.lock()->Play(0, transform.GetWorldPosition(), transform.GetRotation());
}

// 削除処理
void WyvernBallController::OnDelete()
{
	_effectController.lock()->Stop(0);
	_effectController.lock()->Stop(1);
}

// 更新処理
void WyvernBallController::Update(float elapsedTime)
{
	if (_isExploded)
	{
		_lifeTimer -= elapsedTime;
		if (_lifeTimer <= 0.0f)
		{
			GetActor()->Remove();
		}

		return;
	}

	// 火球の位置を更新
	auto& transform = GetActor()->GetTransform();
	Vector3 forward = Vector3::TransformNormal(Vector3::Front, transform.GetMatrix());
	transform.SetPosition(transform.GetPosition() + forward * _ballSpeed * elapsedTime);

	_ballTime -= elapsedTime;
	if (_ballTime <= 0.0f)
	{
		// 火球の進む時間が尽きたら消滅
		_effectController.lock()->Play(1, transform.GetWorldPosition(), transform.GetRotation());
		_isExploded = true;
		// コライダーを大きくする
		_ballCollider.lock()->SetRadius(3.0f);
	}
}

// GUI描画
void WyvernBallController::DrawGui()
{
	ImGui::DragFloat3(u8"火球のオフセット", &_collisionOffset.x, 0.1f, -100.0f, 100.0f);
	ImGui::DragFloat(u8"火球の速度", &_ballSpeed, 1.0f, 0.0f, 1000.0f);
}

// 接触時処理
void WyvernBallController::OnContactEnter(CollisionData& collisionData)
{
	// 攻撃判定
	if (collisionData.myLayer == CollisionLayer::Attack &&
		collisionData.otherLayer == CollisionLayer::Hit &&
		collisionData.other != _ballActor.lock().get())
	{
		// ダメージを与える
		auto damageable = collisionData.other->GetComponent<Damageable>();
		if (damageable != nullptr)
		{
			if (damageable->AddDamage(_ATK, collisionData.hitPosition))
			{
				// ダメージを与えたらヒットエフェクト再生
				auto& transform = GetActor()->GetTransform();
				_effectController.lock()->Play(1, transform.GetWorldPosition(), transform.GetRotation());
				_isExploded = true;

				// コライダーを無効化
				_ballCollider.lock()->SetActive(false);
			}
		}
	}
}
