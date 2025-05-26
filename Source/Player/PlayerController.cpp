#include "PlayerController.h"

#include "../../Source/Enemy/EnemyController.h" 

#include "../../Library/DebugSupporter/DebugSupporter.h"
#include <imgui.h>

void PlayerController::Start()
{
	_charactorController = GetActor()->GetComponent<CharactorController>();
	_animator = GetActor()->GetComponent<Animator>();

	_charactorController.lock()->SetMaxSpeedXZ(5.0f);

    _stateMachine = std::make_unique<PlayerStateMachine>(this, _animator.lock().get());
}

void PlayerController::Update(float elapsedTime)
{
	// 行動処理
    _stateMachine->Execute(elapsedTime);

	// 受けたダメージを初期化
	_stateMachine->SetSustainedDamage(0);
}

// GUI描画
void PlayerController::DrawGui()
{
	if (ImGui::Button(u8"ダメージを受ける"))
	{
		_stateMachine->SetSustainedDamage(1);
	}
	if (ImGui::Button(u8"大ダメージを受ける"))
	{
		_stateMachine->SetSustainedDamage(_stateMachine->GetKnockbackDamage());
	}
	if (ImGui::Button(u8"死亡"))
	{
		_stateMachine->SetIsDead(true);
	}

	ImGui::Separator();
	_stateMachine->DrawGui();
}

// 接触時処理
void PlayerController::OnContact(CollisionData& collisionData)
{
	// 攻撃判定
	if (collisionData.myLayer == CollisionLayer::Attack)
	{
		// 敵にダメージを与える
		auto enemy = collisionData.other->GetComponent<EnemyController>();
		if (enemy != nullptr)
		{
			// 以前に当たっていないか確認
			if (std::find(
				_attackHitActors.begin(), _attackHitActors.end(), enemy->GetActor()->GetName()) 
				!= _attackHitActors.end())
				return;

			enemy->AddDamage(_ATK, collisionData.hitPosition);

			_attackHitActors.push_back(enemy->GetActor()->GetName());
		}
	}
}

// ダメージを与える
void PlayerController::AddDamage(float damage, Vector3 hitPosition)
{
	// 体力を減少
	_health -= damage;
	//// 受けたダメージを加算
	//_sustainedDamage += damage;
	//// ノックバックダメージを加算
	//_knockbackDamage += damage;
	// ダメージを受けた位置を保存
	_hitPosition = hitPosition;
}
