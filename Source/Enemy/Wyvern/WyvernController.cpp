#include "WyvernController.h"

#include "../../Library/Scene/Scene.h"
#include "../../Source/Common/Damageable.h"
#include "../EnemyController.h"

#include <imgui.h>

// 名前取得
void WyvernController::Start()
{
	_enemyController = GetActor()->GetComponent<EnemyController>();
	_behaviorController = GetActor()->GetComponent<BehaviorController>();
	// メタAI取得
	auto metaAIActor = GetActor()->GetScene()->GetActorManager().FindByName("MetaAI", ActorTag::DrawContextParameter);
	if (metaAIActor)
	{
		_metaAI = metaAIActor->GetComponent<MetaAI>();
	}
}
// 更新処理
void WyvernController::Update(float elapsedTime)
{
	if (_enemyController.lock() && _behaviorController.lock() && _behaviorController.lock()->IsExecute())
	{
		// メタAIからターゲット座標を取得
		if (auto metaAI = _metaAI.lock())
		{
			auto targetable = metaAI->SearchTarget(
				Targetable::Faction::Player,
				GetActor()->GetTransform().GetWorldPosition(),
				100.0f);
			if (targetable)
			{
				_enemyController.lock()->SetTargetPosition(targetable->GetActor()->GetTransform().GetWorldPosition());
				_enemyController.lock()->SetInFighting(true);
			}
			else
			{
				// 戦闘状態を解除
				_enemyController.lock()->SetInFighting(false);
			}
		}
	}
}
// GUI描画
void WyvernController::DrawGui()
{
	ImGui::DragFloat(u8"近接攻撃角度", &_nearAttackRadian, 0.01f, 0.0f, DirectX::XM_PI, "%.1f rad");
}
