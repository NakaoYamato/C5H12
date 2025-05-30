#include "PlayerController.h"

#include "../../Source/Enemy/EnemyController.h" 
#include "../../Source/Common/Damageable.h"

#include "../../Library/Scene/Scene.h"
#include "../../Library/DebugSupporter/DebugSupporter.h"
#include <PlayerDefine.h>
#include <imgui.h>

void PlayerController::Start()
{
	_charactorController = GetActor()->GetComponent<CharactorController>();
	_animator = GetActor()->GetComponent<Animator>();
	_hitEffectController = GetActor()->GetComponent<EffekseerEffectController>();
	_damageable = GetActor()->GetComponent<Damageable>();
	_damageable.lock()->SetTakeableDamageCallback(
		[&](float damage, Vector3 hitPosition) -> bool
		{
			Vector3 vec = hitPosition - GetActor()->GetTransform().GetPosition();
			Vector3 front = GetActor()->GetTransform().GetAxisZ();
			// プレイヤーがガード状態ならダメージを受けない
			if (_stateMachine->GetStateName() == Network::GetPlayerMainStateName(Network::PlayerMainStates::Guard) &&
				vec.Dot(front) > 0.0f)
			{
				// ガード成功
				_stateMachine->ChangeState(Network::PlayerMainStates::GuardHit, Network::PlayerSubStates::None);
				return false;
			}
			return true;
		}
	);
	_damageable.lock()->SetOnDamageCallback(
		[&](float damage, Vector3 hitPosition)
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
	);

	_charactorController.lock()->SetMaxSpeedXZ(5.0f);

    _stateMachine = std::make_unique<PlayerStateMachine>(this, _animator.lock().get());
}

// 更新処理
void PlayerController::Update(float elapsedTime)
{
	// 行動処理
    _stateMachine->Execute(elapsedTime);
}

// 3D描画後の描画処理
void PlayerController::DelayedRender(const RenderContext& rc)
{
	// 頭上に名前表示
	GetActor()->GetScene()->GetTextRenderer().Draw3D(
		FontType::MSGothic,
		GetActor()->GetName(),
		GetActor()->GetTransform().GetPosition() + Vector3(0.0f, 2.0f, 0.0f),
		Vector4::White
	);
}

// GUI描画
void PlayerController::DrawGui()
{
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
