#include "WyvernController.h"

#include "../../Library/Scene/Scene.h"
#include "../../Source/Common/Damageable.h"
#include "../EnemyController.h"
#include "WyvernActor.h"
#include <imgui.h>

// 名前取得
void WyvernController::Start()
{
	_charactorController = GetActor()->GetComponent<CharactorController>();
	_enemyController = GetActor()->GetComponent<EnemyController>();
	_effectController = GetActor()->GetComponent<EffectController>();
	_behaviorController = GetActor()->GetComponent<BehaviorController>();
	_combatStatus = GetActor()->GetComponent<CombatStatusController>();
	_damageable = GetActor()->GetComponent<Damageable>();
	_staminaController = GetActor()->GetComponent<StaminaController>();
	// メタAI取得
	auto metaAIActor = GetActor()->GetScene()->GetActorManager().FindByName("MetaAI", ActorTag::System);
	if (metaAIActor)
	{
		_metaAI = metaAIActor->GetComponent<MetaAI>();
	}

	// ダメージ間隔を設定
	_enemyController.lock()->SetDamageReactionRate(3.0f);
	_enemyController.lock()->SetDownDamageReactionRate(5.0f);

	// 初期スキン幅取得
	_initialSkinWidth = _charactorController.lock()->GetSkinWidth();

	// ワイバーンの頭の位置を取得
	auto model = GetActor()->GetModel().lock();
	_headNodeIndex = model->GetNodeIndex("Head");
}
// 更新処理
void WyvernController::Update(float elapsedTime)
{
	_combatStatus.lock()->SetIsUpdate(
		_enemyController.lock() && _behaviorController.lock() && _behaviorController.lock()->IsExecute()
	);

	// 空中処理
	if (_isDuringFlight)
	{
		_charactorController.lock()->SetSkinWidth(_flightSkinWidth);
		_flightTimer += elapsedTime;
		// 空中時はスタミナが自動回復しない
		_staminaController.lock()->SetIsStaminaRecover(false);
	}
	else
	{
		_charactorController.lock()->SetSkinWidth(_initialSkinWidth);
		_flightTimer = 0.0f;
		_staminaController.lock()->SetIsStaminaRecover(true);
	}

	// 怒り状態
	if (_enemyController.lock()->IsAngry())
	{
		// 口の中にエフェクト再生
		_mouthBreathEffectTimer += elapsedTime;
		if (_mouthBreathEffectTimer >= _mouthBreathEffectInterval)
		{
			_mouthBreathEffectTimer -= _mouthBreathEffectInterval;
			auto model = GetActor()->GetModel().lock();
			DirectX::XMFLOAT4X4 worldMatrix = model->GetPoseNodes()[_headNodeIndex].worldTransform;
			_effectController.lock()->Play(WyvernActor::EffectType::MouthBreathEffect, _mouthBreathEffectOffset.TransformCoord(worldMatrix));
		}
	}

	// 体力が初めて85%以下になったときに怒り移行
	if (auto damageable = _damageable.lock())
	{
		if (damageable->GetHealth() <= 85.0f && !_enemyController.lock()->IsAngry())
		{
			_enemyController.lock()->SetIsAngry(true);
		}
	}
}
// GUI描画
void WyvernController::DrawGui()
{
	ImGui::DragFloat(u8"近接攻撃角度", &_nearAttackRadian, 0.01f, 0.0f, DirectX::XM_PI, "%.1f rad");
	ImGui::DragFloat(u8"空中スキン幅", &_flightSkinWidth, 0.1f, 0.0f, 20.0f, "%.1f");
	ImGui::Checkbox(u8"空中か", &_isDuringFlight);
	ImGui::DragFloat(u8"飛行中近距離判定距離", &_flightNearRange, 0.1f, 0.0f, 100.0f, "%.1f");
	ImGui::DragFloat(u8"飛行中移動速度", &_flightMoveSpeed, 0.1f, 0.0f, 50.0f, "%.1f");
	ImGui::DragFloat(u8"飛行中タイマー", &_flightTimer, 0.1f, 0.0f, 100.0f, "%.1f");
	ImGui::DragFloat(u8"飛行継続時間", &_flightDuration, 0.1f, 0.0f, 100.0f, "%.1f");
	ImGui::Separator();
	ImGui::DragFloat(u8"近接攻撃チャージ時間", &_chargeAttackChargeTime, 0.1f, 0.0f, 10.0f, "%.1f");
	ImGui::Separator();
	ImGui::DragFloat(u8"口炎エフェクト間隔", &_mouthBreathEffectInterval, 0.1f, 0.0f, 10.0f, "%.1f");
	ImGui::DragFloat3(u8"口炎エフェクトオフセット", &_mouthBreathEffectOffset.x, 0.1f, -100.0f, 100.0f, "%.1f");
}
