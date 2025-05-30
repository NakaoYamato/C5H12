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
			// ƒvƒŒƒCƒ„[‚ªƒK[ƒhó‘Ô‚È‚çƒ_ƒ[ƒW‚ðŽó‚¯‚È‚¢
			if (_stateMachine->GetStateName() == Network::GetPlayerMainStateName(Network::PlayerMainStates::Guard) &&
				vec.Dot(front) > 0.0f)
			{
				// ƒK[ƒh¬Œ÷
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
				// ‘å‚«‚­‚Ì‚¯‚¼‚é
				_stateMachine->ChangeState(Network::PlayerMainStates::HitKnockDown, Network::PlayerSubStates::None);
			}
			else
			{
				// Œy‚­‚Ì‚¯‚¼‚é
				_stateMachine->ChangeState(Network::PlayerMainStates::Hit, Network::PlayerSubStates::None);
			}
		}
	);

	_charactorController.lock()->SetMaxSpeedXZ(5.0f);

    _stateMachine = std::make_unique<PlayerStateMachine>(this, _animator.lock().get());
}

// XVˆ—
void PlayerController::Update(float elapsedTime)
{
	// s“®ˆ—
    _stateMachine->Execute(elapsedTime);

	// UŒ‚—Í‚ÌXV
	_ATK = _BaseATK * _ATKFactor;
}

// 3D•`‰æŒã‚Ì•`‰æˆ—
void PlayerController::DelayedRender(const RenderContext& rc)
{
	// “ªã‚É–¼‘O•\Ž¦
	GetActor()->GetScene()->GetTextRenderer().Draw3D(
		FontType::MSGothic,
		GetActor()->GetName(),
		GetActor()->GetTransform().GetPosition() + Vector3(0.0f, 2.0f, 0.0f),
		Vector4::White
	);
}

// GUI•`‰æ
void PlayerController::DrawGui()
{
	if (ImGui::Button(u8"Ž€–S"))
	{
		_stateMachine->SetIsDead(true);
	}
	ImGui::DragFloat(u8"UŒ‚—Í",		&_ATK, 0.1f, 0.0f, 100.0f);
	ImGui::DragFloat(u8"Šî–{UŒ‚—Í", &_BaseATK, 0.1f, 0.0f, 100.0f);
	ImGui::DragFloat(u8"UŒ‚”{—¦",	&_ATKFactor, 0.01f, 0.0f, 10.0f);

	ImGui::Separator();
	_stateMachine->DrawGui();
}

// ÚGŽžˆ—
void PlayerController::OnContactEnter(CollisionData& collisionData)
{
	// UŒ‚”»’è
	if (collisionData.myLayer == CollisionLayer::Attack)
	{
		// ƒ_ƒ[ƒW‚ð—^‚¦‚é
		auto damageable = collisionData.other->GetComponent<Damageable>();
		if (damageable != nullptr)
		{
			if (damageable->AddDamage(_ATK, collisionData.hitPosition))
			{
				// ƒ_ƒ[ƒW‚ð—^‚¦‚½‚çƒqƒbƒgƒGƒtƒFƒNƒgÄ¶
				_hitEffectController.lock()->Play(collisionData.hitPosition, 1.0f);
			}
		}
	}
}
