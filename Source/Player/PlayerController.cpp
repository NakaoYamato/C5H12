#include "PlayerController.h"

#include "../../Library/Scene/Scene.h"
#include "../../Source/Enemy/EnemyController.h" 

#include <imgui.h>

void PlayerController::Start()
{
	_charactorController = GetActor()->GetComponent<CharactorController>();
	_animator = GetActor()->GetComponent<Animator>();
	_effectController = GetActor()->GetComponent<EffectController>();
	_damageable = GetActor()->GetComponent<Damageable>();
	_targetable = GetActor()->GetComponent<Targetable>();

	auto stateController = GetActor()->GetComponent<StateController>();
	_stateMachine = std::dynamic_pointer_cast<PlayerStateMachine>(stateController->GetStateMachine());

	_damageable.lock()->SetTakeableDamageCallback(
		[&](float damage, Vector3 hitPosition) -> bool
		{
			Vector3 vec = hitPosition - GetActor()->GetTransform().GetPosition();
			Vector3 front = GetActor()->GetTransform().GetAxisZ();
			// プレイヤーがガード状態ならダメージを受けない
			if (_stateMachine.lock()->GetStateName() == "Guard" &&
				vec.Dot(front) > 0.0f)
			{
				// ガード成功
				//_stateMachine.lock()->GetStateMachine().ChangeSubState("GuardHit");
				_stateMachine.lock()->ChangeState("GuardHit", nullptr);
				return false;
			}
			return true;
		}
	);
	_damageable.lock()->SetOnDamageCallback(
		[&](float damage, Vector3 hitPosition)
		{
			bool isCombat = false;
			{
				if (std::string(_stateMachine.lock()->GetStateName()).find("Combat") != std::string::npos)
					isCombat = true;
			}

			if (damage >= 5.0f)
			{
				// 大きくのけぞる
				if (isCombat)
					_stateMachine.lock()->ChangeState("CombatDown", nullptr);
				else
					_stateMachine.lock()->ChangeState("Down", nullptr);
			}
			else if (damage >= 2.0f)
			{
				// 中くらいにのけぞる
				if (isCombat)
					_stateMachine.lock()->ChangeState("CombatHitKnockDown", nullptr);
				else
					_stateMachine.lock()->ChangeState("HitKnockDown", nullptr);
			}
			else
			{
				// 軽くのけぞる
				if (isCombat)
					_stateMachine.lock()->ChangeState("CombatHit", nullptr);
				else
					_stateMachine.lock()->ChangeState("Hit", nullptr);
			}
		}
	);

	_charactorController.lock()->SetMaxSpeedXZ(5.0f);
}

// 更新処理
void PlayerController::Update(float elapsedTime)
{
	auto animator = _animator.lock();

	_callInputBufferingEvent = false;
	_callCancelAttackEvent = false;
	_callCancelEvent = false;
	_oldInvisibleEvent = _callInvisivleEvent;
	_callInvisivleEvent = false;
	_callChargingEvent = false;

	// アニメーションイベント取得
	if (animator->IsPlayAnimation())
	{
		auto& animationEvent = animator->GetAnimationEvent();
		int massageListSize = (int)animationEvent.GetMessageList().size();
		auto events = animator->GetCurrentEvents();
		for (auto& event : events)
		{
			// メッセージインデックスが範囲外ならcontinue
			if (event.messageIndex < 0 || event.messageIndex >= massageListSize)
				continue;

            // 先行入力判定
			if (animationEvent.GetMessageList().at(event.messageIndex) == "InputBuffering")
			{
				_callInputBufferingEvent = true;
			}
			// 攻撃キャンセル判定
			else if (animationEvent.GetMessageList().at(event.messageIndex) == "CancelAttack")
			{
				_callCancelAttackEvent = true;
			}
			// キャンセル判定
			else if (animationEvent.GetMessageList().at(event.messageIndex) == "Cancel")
			{
				_callCancelEvent = true;
			}
			// 無敵判定
			else if (animationEvent.GetMessageList().at(event.messageIndex) == "Invisible")
			{
				_callInvisivleEvent = true;
			}
			// 溜め
			else if (animationEvent.GetMessageList().at(event.messageIndex) == "Charging")
			{
				_callChargingEvent = true;
			}
		}
	}
}

// 3D描画後の描画処理
void PlayerController::DelayedRender(const RenderContext& rc)
{
	// 頭上に名前表示
	//GetActor()->GetScene()->GetTextRenderer().Draw3D(
	//	FontType::MSGothic,
	//	GetActor()->GetName(),
	//	GetActor()->GetTransform().GetPosition() + Vector3(0.0f, 2.0f, 0.0f),
	//	Vector4::White
	//);
}

// GUI描画
void PlayerController::DrawGui()
{
	ImGui::Separator();
}
