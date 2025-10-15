#include "WyvernController.h"

#include "../../Library/Scene/Scene.h"
#include "../../Source/Common/Damageable.h"
#include "../EnemyController.h"
#include "../../Library/Component/Terrain/TerrainEnvironmentController.h"
#include "../../Library/DebugSupporter/DebugSupporter.h"

#include <imgui.h>

// 名前取得
void WyvernController::Start()
{
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
	_enemyController.lock()->SetDamageReactionRate(5.0f);
}
// 更新処理
void WyvernController::Update(float elapsedTime)
{
	_combatStatus.lock()->SetIsUpdate(
		_enemyController.lock() && _behaviorController.lock() && _behaviorController.lock()->IsExecute()
	);
}
// GUI描画
void WyvernController::DrawGui()
{
	ImGui::DragFloat(u8"近接攻撃角度", &_nearAttackRadian, 0.01f, 0.0f, DirectX::XM_PI, "%.1f rad");
}
// オブジェクトとの接触した瞬間時の処理
void WyvernController::OnContactEnter(CollisionData& collisionData)
{
	auto environment = collisionData.other->GetComponent<TerrainEnvironmentController>();
	if (environment && !collisionData.otherIsTrigger)
	{
		Debug::Output::String(L"地形オブジェクトに接触\n");
		collisionData.other->Remove();
		return;
	}
}
