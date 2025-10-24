#include "WyvernController.h"

#include "../../Library/Scene/Scene.h"
#include "../../Source/Common/Damageable.h"
#include "../EnemyController.h"
#include <imgui.h>

// 名前取得
void WyvernController::Start()
{
	_charactorController = GetActor()->GetComponent<CharactorController>();
	_enemyController = GetActor()->GetComponent<EnemyController>();
	_behaviorController = GetActor()->GetComponent<BehaviorController>();
	_combatStatus = GetActor()->GetComponent<CombatStatusController>();
	// メタAI取得
	auto metaAIActor = GetActor()->GetScene()->GetActorManager().FindByName("MetaAI", ActorTag::DrawContextParameter);
	if (metaAIActor)
	{
		_metaAI = metaAIActor->GetComponent<MetaAI>();
	}

	// ダメージ間隔を設定
	_enemyController.lock()->SetDamageReactionRate(3.0f);
	_enemyController.lock()->SetDownDamageReactionRate(5.0f);

	// 初期スキン幅取得
	_initialSkinWidth = _charactorController.lock()->GetSkinWidth();
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
	}
	else
	{
		_charactorController.lock()->SetSkinWidth(_initialSkinWidth);
		_flightTimer = 0.0f;
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
}
