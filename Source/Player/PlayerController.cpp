#include "PlayerController.h"

#include "../../Library/Scene/Scene.h"
#include "../../Source/Enemy/EnemyController.h" 
#include "../../Source/Common/DamageSender.h"
#include "../../Source/Common/InteractionController.h"

#include "../../Source/Camera/PlayerCameraController.h"
#include "../../Source/Camera/LockOnCamera.h"
#include "../../Source/Camera/PlayerDeathCamera.h"

#include <imgui.h>

void PlayerController::Start()
{
	_charactorController = GetActor()->GetComponent<CharactorController>();
	_animator = GetActor()->GetComponent<Animator>();
	_effectController = GetActor()->GetComponent<EffectController>();
	_modelRenderer = GetActor()->GetComponent<ModelRenderer>();

	_damageable = GetActor()->GetComponent<Damageable>();
	auto damageSender = GetActor()->GetComponent<DamageSender>();
	_targetable = GetActor()->GetComponent<Targetable>();

	_playerItemController = GetActor()->GetComponent<PlayerItemController>();

	auto stateController = GetActor()->GetComponent<StateController>();
	_stateMachine = std::dynamic_pointer_cast<PlayerStateMachine>(stateController->GetStateMachine());

	_inputManager = GetActor()->GetScene()->GetActorManager().FindByClass<InputManager>(ActorTag::System);

	// メタAI取得
	auto metaAIActor = GetActor()->GetScene()->GetActorManager().FindByName("MetaAI", ActorTag::System);
	if (metaAIActor)
	{
		_metaAI = metaAIActor->GetComponent<MetaAI>();

		// メタAIからリスポーン位置を取得
		EntryZone* entryZone = _metaAI.lock()->SearchNearestEntryZone(Targetable::Faction::Player, GetActor()->GetTransform().GetWorldPosition());
		if (entryZone)
		{
			Vector3 respawnPosition = entryZone->GetActor()->GetTransform().GetWorldPosition() + entryZone->GetCenter();
			Vector3 respawnAngle = entryZone->GetAngle();
			// 座標と角度を設定
			GetActor()->GetTransform().SetWorldPosition(respawnPosition);
			GetActor()->GetTransform().SetWorldAngle(respawnAngle);
		}
	}
#pragma region コールバック設定
	// 被ダメージコールバック設定
	_damageable.lock()->RegisterTakeableDamageCallback(
		"PlayerController",
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
	// ダメージ時コールバック設定
	_damageable.lock()->RegisterOnDamageCallback(
		"PlayerController",
		[&](float damage, Vector3 hitPosition) -> void
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
	// 死亡時のコールバック設定
	_damageable.lock()->RegisterOnDeathCallback(
		"PlayerController",
		[&]() -> void
		{
			// 死亡ステートへ変更
			_stateMachine.lock()->ChangeState("Death", nullptr);
			// ステートの変更を受け付けないようにする
			_stateMachine.lock()->SetCanChangeState(false);

			// カメラを死亡カメラに変更
			auto deathCamera = GetActor()->GetScene()->GetMainCameraActor()->GetControllerByClass<PlayerDeathCamera>();
			deathCamera->SetNextControllerName("PlayerCameraController");
			deathCamera->Swich();

			_respawnTimer = 0.0f;
		}
	);

	// ダメージを与えた時のコールバック設定
	damageSender->SetOnSendDamageCallback(
		"PlayerController",
		[&](DamageSender* myself, CollisionData& collisionData)
		{
			// ゲームパッド振動
			SetVibration(_stageContactVibrationL, _stageContactVibrationR, _stageContactVibrationTime);
			// ヒットストップ
			GetActor()->GetScene()->GetActorManager().SetGameSpeed(ActorTag::Player, _hitStopScale, _hitStopTime);
		}
	);

	// ステージ接触時のコールバック設定
	damageSender->SetOnStageContactCallback(
		"PlayerController",
		[&](StageEffectEmitter* target, CollisionData& collisionData)
		{
			// ゲームパッド振動
			SetVibration(_stageContactVibrationL, _stageContactVibrationR, _stageContactVibrationTime);
		}
	);
#pragma endregion

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
	_chargeEffectRimLightColor2.w = 2.0f;
	_chargeEffectRimLightColor3.w = 3.0f;
}

// 更新処理
void PlayerController::Update(float elapsedTime)
{
	// 死亡している時
	if (_damageable.lock()->IsDead())
	{
		_respawnTimer += elapsedTime;
		if (_respawnTimer >= _respawnTimeMax)
		{
			// メタAIからリスポーン位置を取得
			EntryZone* entryZone = _metaAI.lock()->SearchNearestEntryZone(Targetable::Faction::Player, GetActor()->GetTransform().GetWorldPosition());
			if (entryZone)
			{
				Vector3 respawnPosition = entryZone->GetActor()->GetTransform().GetWorldPosition() + entryZone->GetCenter();
				Vector3 respawnAngle = entryZone->GetAngle();
				Respawn(respawnPosition, respawnAngle);
			}
			else
			{
				// リスポーン位置が見つからなかった場合、現在位置にリスポーン
				Respawn(GetActor()->GetTransform().GetWorldPosition(), GetActor()->GetTransform().GetWorldAngle());
			}
		}

		return;
	}

	auto animator = _animator.lock();

	_callInputBufferingEvent = false;
	_callCancelAttackEvent = false;
	_callCancelEvent = false;
	_oldInvisibleEvent = _callInvisivleEvent;
	_callInvisivleEvent = false;
	_callChargingEvent = false;

	// アニメーションイベント取得
	if (animator->IsPlaying())
	{
		auto& animationEvent = animator->GetAnimationEvent();
		int massageListSize = (int)animationEvent.GetMessageList().size();
		auto events = animator->GetCurrentEvents();
		for (auto& event : events)
		{
			if (event.eventType == AnimationEvent::EventType::Attack)
			{
				// モーション値取得
				_attackMotionFactor = static_cast<float>(event.messageIndex);
				continue;
			}

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

	// 最後に移動してからの時間
	if (_isMoving)
		_timeSinceLastMove = 0.0f;
	else
		_timeSinceLastMove += elapsedTime;

	// ロックオンカメラ遷移
	if (_isLockOnCameraTransition)
	{
		auto lockOnCamera = GetActor()->GetScene()->GetMainCameraActor()->GetControllerByClass<LockOnCamera>();
		if (lockOnCamera)
		{
			// ロックオンカメラを起動しているかどうか
			if (GetActor()->GetScene()->GetMainCameraActor()->GetCurrentControllerName() == "LockOnCamera")
			{
				// プレイヤーカメラに戻す
				lockOnCamera->SetTarget(nullptr);
				GetActor()->GetScene()->GetMainCameraActor()->SwitchController("PlayerCameraController");
			}
			else
			{
				// メタAIからターゲットを取得
				auto target = _metaAI.lock()->SearchTarget(Targetable::Faction::Enemy,
					GetActor()->GetTransform().GetWorldPosition(),
					100.0f);
				if (target)
				{
					// ロックオンカメラに切り替え
					lockOnCamera->SetTarget(target);
					lockOnCamera->Swich();
				}
			}
		}

		_isLockOnCameraTransition = false;
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
	ImGui::Separator();
	ImGui::DragFloat(u8"ヒットストップ時間", &_hitStopTime, 0.01f, 0.0f, 1.0f);
	ImGui::DragFloat(u8"ヒットストップスケール", &_hitStopScale, 0.01f, 0.0f, 1.0f);
	ImGui::Separator();
	ImGui::DragFloat(u8"ステージ接触時のLモーター値", &_stageContactVibrationL, 0.01f, 0.0f, 1.0f);
	ImGui::DragFloat(u8"ステージ接触時のRモーター値", &_stageContactVibrationR, 0.01f, 0.0f, 1.0f);
	ImGui::DragFloat(u8"ステージ接触時の時間", &_stageContactVibrationTime, 0.01f, 0.0f, 1.0f);
	ImGui::Separator();
	ImGui::DragFloat(u8"リスポーン時間", &_respawnTimeMax, 0.1f, 0.0f, 10.0f);
}

// 接触処理
void PlayerController::OnContact(CollisionData& collisionData)
{
	if (collisionData.otherLayer == CollisionLayer::Stage &&
		collisionData.otherIsTrigger)
	{
		// インタラクティブオブジェクトとの接触
		if (auto interactionController = collisionData.other->GetComponent<InteractionController>())
		{
			// 選択可能かどうかの判定
			if (interactionController->IsSelectable(GetActor().get()))
			{
				interactionController->OnSelect(GetActor().get());

				// 使用可能かどうかの判定
				if (interactionController->IsUsable(GetActor().get()))
				{
					// 納刀状態、アイテム使用中でない場合のみ可能
					if (!IsDrawingWeapon() && !IsUsingItem())
					{
						if (IsSelect())
						{
							interactionController->OnUse(GetActor().get());
						}
					}
				}
			}
		}
	}
}

// ゲームパッド振動
void PlayerController::SetVibration(float left, float right, float time)
{
	if (auto inputManager = _inputManager.lock())
	{
		inputManager->SetVibration(left, right, time);
	}
}

// テレポート
void PlayerController::Teleport(const Vector3& position, const Vector3& angle)
{
	// 座標と角度を設定
	GetActor()->GetTransform().SetWorldPosition(position);
	GetActor()->GetTransform().SetWorldAngle(angle);

	// カメラを更新
	// 通常カメラに切り替え
	auto playerCamera = GetActor()->GetScene()->GetMainCameraActor()->GetControllerByClass<PlayerCameraController>();
	playerCamera->Swich();
	playerCamera->Reset(angle);
}

// リスポーン
void PlayerController::Respawn(const Vector3& position, const Vector3& angle)
{
	// 体力を全回復
	_damageable.lock()->ResetHealth(_damageable.lock()->GetMaxHealth());

	// ステートの変更を受け付けるようにする
	_stateMachine.lock()->SetCanChangeState(true);
	_stateMachine.lock()->ChangeState("Idle", nullptr);

	// テレポート
	Teleport(position, angle);
}
