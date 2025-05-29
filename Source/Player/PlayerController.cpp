#include "PlayerController.h"

#include "../../Source/Enemy/EnemyController.h" 

#include "../../Library/DebugSupporter/DebugSupporter.h"
#include <PlayerDefine.h>
#include <imgui.h>

void PlayerController::Start()
{
	_charactorController = GetActor()->GetComponent<CharactorController>();
	_animator = GetActor()->GetComponent<Animator>();
	_hitEffectController = GetActor()->GetComponent<EffekseerEffectController>();

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
void PlayerController::OnContactEnter(CollisionData& collisionData)
{
	// 攻撃判定
	if (collisionData.myLayer == CollisionLayer::Attack)
	{
		// 敵にダメージを与える
		auto enemy = collisionData.other->GetComponent<EnemyController>();
		if (enemy != nullptr)
		{
			if (enemy->AddDamage(_ATK, collisionData.hitPosition))
			{
				// ダメージを与えたらヒットエフェクト再生
				_hitEffectController.lock()->Play(collisionData.hitPosition, 1.0f);
			}
		}
	}
}

// ダメージを与える
bool PlayerController::AddDamage(float damage, Vector3 hitPosition)
{
	Vector3 vec = hitPosition - GetActor()->GetTransform().GetPosition();
	Vector3 front = GetActor()->GetTransform().GetAxisZ();

	// プレイヤーがガード状態で攻撃地点がプレイヤーの前方向ならガード処理
	if (_stateMachine->GetStateName() == Network::GetPlayerMainStateName(Network::PlayerMainStates::Guard) &&
		vec.Dot(front) > 0.0f)
	{
		// ガード成功
		_stateMachine->ChangeState(Network::PlayerMainStates::GuardHit, Network::PlayerSubStates::None);
		return false;
	}
	// 通常処理
	return IDamagable::AddDamage(damage, hitPosition);
}

// ダメージを受けた時の処理
void PlayerController::OnDamage(float damage, Vector3 hitPosition)
{
	if (damage >= 2.0f)
	{
		// 大きくのけぞる
		_stateMachine->ChangeState(Network::PlayerMainStates::HitKnockDown, Network::PlayerSubStates::None);
	}
	else
	{
		// 軽くのけぞる
		_stateMachine->ChangeState(Network::PlayerMainStates::Hit, Network::PlayerSubStates::None);
	}
}
