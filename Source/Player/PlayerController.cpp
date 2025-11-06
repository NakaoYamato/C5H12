#include "PlayerController.h"

#include "../../Library/Scene/Scene.h"
#include "../../Source/Enemy/EnemyController.h" 

#include "../../Source/Stage/Props/Chest/ChestController.h"

#include <imgui.h>

void PlayerController::Start()
{
	_charactorController = GetActor()->GetComponent<CharactorController>();
	_animator = GetActor()->GetComponent<Animator>();
	_effectController = GetActor()->GetComponent<EffectController>();
	_modelRenderer = GetActor()->GetComponent<ModelRenderer>();

	_damageable = GetActor()->GetComponent<Damageable>();
	_targetable = GetActor()->GetComponent<Targetable>();

	auto stateController = GetActor()->GetComponent<StateController>();
	_stateMachine = std::dynamic_pointer_cast<PlayerStateMachine>(stateController->GetStateMachine());

	_damageable.lock()->SetTakeableDamageCallback(
		[&](float damage, Vector3 hitPosition) -> bool
		{
			Vector3 vec = hitPosition - GetActor()->GetTransform().GetPosition().Normalize();
			Vector3 front = GetActor()->GetTransform().GetAxisZ().Normalize();
			// プレイヤーがガード状態ならダメージを受けない
			if (_stateMachine.lock()->GetStateName() == "CombatGuard" &&
				vec.Dot(front) > -0.5f)
			{
				// ガード成功
				//_stateMachine.lock()->GetStateMachine().ChangeSubState("GuardHit");
				_stateMachine.lock()->GetStateMachine().ChangeSubState("GuardHit");
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

	// リムライト初期化
	if (_modelRenderer.lock())
	{
		for (auto& material : _modelRenderer.lock()->GetMaterials())
		{
			Vector4 color = Vector4::Black;
			color.w = 0.0f;
			material.SetParameter("bodyColor", color);
		}
	}
	_chargeEffectRimLightColor1.w = 2.0f;
	_chargeEffectRimLightColor2.w = 8.0f;
	_chargeEffectRimLightColor3.w = 10.0f;
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

	// リムライト処理
	if (_modelRenderer.lock() && _chargeEffectRimLightTimer > 0.0f)
	{
		_chargeEffectRimLightTimer -= elapsedTime;
		if (_chargeEffectRimLightTimer < 0.0f)
			_chargeEffectRimLightTimer = 0.0f;

		float rate = std::clamp(_chargeEffectRimLightTimer / _chargeEffectRimLightTimeMax, 0.0f, 1.0f);

		for (auto& material : _modelRenderer.lock()->GetMaterials())
		{
			Vector4 color = Vector4::Black;
			switch (_chargeLevel)
			{
			case 1:
				color = _chargeEffectRimLightColor1;
				break;
			case 2:
				color = _chargeEffectRimLightColor2;
				break;
			case 3:
				color = _chargeEffectRimLightColor3;
				break;
			}
			color.w = color.w * rate;
			material.SetParameter("bodyColor", color);
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
    ImGui::DragFloat(u8"ダッシュ回転速度係数", &_dashRotationFactor, 0.1f, 0.0f, 20.0f);
	ImGui::Separator();
    ImGui::DragFloat(u8"ダッシュスタミナ消費量", &_dashStaminaConsume, 0.1f, 0.0f, 100.0f);
    ImGui::DragFloat(u8"回避スタミナ消費量", &_evadeStaminaConsume, 0.1f, 0.0f, 100.0f);
    ImGui::DragFloat(u8"ガードスタミナ消費量", &_guardStaminaConsume, 0.1f, 0.0f, 100.0f);
	ImGui::Separator();
	ImGui::Text(u8"溜め段階: %d", _chargeLevel);
	ImGui::Separator();
	ImGui::DragFloat(u8"溜め演出リムライト表示経過時間", &_chargeEffectRimLightTimer, 0.1f, 0.0f, 5.0f);
	ImGui::DragFloat(u8"溜め演出リムライト表示時間", &_chargeEffectRimLightTimeMax, 0.1f, 0.0f, 5.0f);
	ImGui::ColorEdit3(u8"溜め段階1リムライトカラー", &_chargeEffectRimLightColor1.x);
	ImGui::DragFloat(u8"溜め段階1リムライトカラー強度", &_chargeEffectRimLightColor1.w, 0.01f, 0.0f, 10.0f);
	ImGui::ColorEdit3(u8"溜め段階2リムライトカラー", &_chargeEffectRimLightColor2.x);
	ImGui::DragFloat(u8"溜め段階2リムライトカラー強度", &_chargeEffectRimLightColor2.w, 0.01f, 0.0f, 10.0f);
	ImGui::ColorEdit3(u8"溜め段階3リムライトカラー", &_chargeEffectRimLightColor3.x);
	ImGui::DragFloat(u8"溜め段階3リムライトカラー強度", &_chargeEffectRimLightColor3.w, 0.01f, 0.0f, 10.0f);
}

// 接触処理
void PlayerController::OnContact(CollisionData& collisionData)
{
	if (collisionData.otherLayer == CollisionLayer::Stage &&
		collisionData.otherIsTrigger)
	{
		auto chestController = collisionData.other->GetComponent<ChestController>();
		if (chestController)
		{
			if (IsSelect())
				chestController->Open();
		}
	}
}
