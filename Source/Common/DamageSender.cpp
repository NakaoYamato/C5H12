#include "DamageSender.h"

#include "../../Source/Common/Damageable.h"

#include <imgui.h>

// 開始処理
void DamageSender::Start()
{
	_modelCollider = GetActor()->GetCollider<ModelCollider>();
	_effectController = GetActor()->GetComponent<EffectController>();
	_targetable = GetActor()->GetComponent<Targetable>();
}

// 更新処理
void DamageSender::Update(float elapsedTime)
{
	if (auto modelCollider = _modelCollider.lock())
	{
		// 攻撃イベントが呼び出されていなければリセット
		if (!modelCollider->IsCollAttackEvent())
			ResetAttackState();
	}
}

// GUI描画
void DamageSender::DrawGui()
{
	ImGui::DragFloat(u8"基本攻撃力", &_baseATK, 0.1f, 0.0f, 100.0f);
	ImGui::DragFloat(u8"モーション値", &_motionFactor, 0.01f, 0.0f, 10.0f);
	ImGui::DragFloat(u8"攻撃倍率", &_sharpnessFactor, 0.01f, 0.0f, 10.0f);
	ImGui::DragFloat(u8"会心倍率", &_criticalFactor, 0.01f, 0.0f, 10.0f);
	ImGui::DragInt(u8"ヒットエフェクトインデックス", &_hitEffectIndex, 1, -1, 100);
	ImGui::DragFloat(u8"ヘイト倍率", &_heteFactor, 0.01f, 0.0f, 10.0f);
}

// 接触時処理
void DamageSender::OnContact(CollisionData& collisionData)
{
	// 攻撃判定
	if (collisionData.myLayer == CollisionLayer::Attack)
	{
		// 攻撃先情報を取得
		// もし親がいるならその親の名前で取得
		std::string targetName = collisionData.other->GetParent() ?
			collisionData.other->GetParent()->GetName() :
			collisionData.other->GetName();

		// 以前に攻撃していた相手には攻撃しない
		if (_attackedTargets.find(targetName) != _attackedTargets.end())
			return;

		// ダメージを与える
		auto damageable = collisionData.other->GetComponent<Damageable>();
		if (damageable != nullptr)
		{
			// 攻撃力計算
			CalculateATK(1.0f/*TODO : 肉質実装*/);

			if (damageable->AddDamage(_ATK, collisionData.hitPosition))
			{
				// ダメージを与えたらヒットエフェクト再生
				if (_effectController.lock() && _hitEffectIndex != -1)
					_effectController.lock()->Play(_hitEffectIndex, collisionData.hitPosition);
				// 自身のヘイト値を増やす
				_targetable.lock()->AddHateValue(_ATK);

				// 攻撃先情報を保存
				_attackedTargets[targetName] = collisionData.other;
			}
		}
	}
}

// 攻撃力の計算
void DamageSender::CalculateATK(float hitzoneFactor)
{
	_ATK = _baseATK * _motionFactor * _sharpnessFactor * hitzoneFactor * _criticalFactor;
}
