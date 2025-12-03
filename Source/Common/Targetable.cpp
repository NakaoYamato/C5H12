#include "Targetable.h"

#include "../../Library/Scene/Scene.h"
#include "../../Source/AI/MetaAI.h"
#include "../../Source/Stage/StageController.h"

#include "../../External/nameof/include/nameof.hpp"
#include "../../Source/Stage/SafetyZone.h"
#include <imgui.h>

// 開始処理
void Targetable::Start()
{
	// メタAIを取得
	auto metaAIActor = GetActor()->GetScene()->GetActorManager().FindByName("MetaAI", ActorTag::System);
	if (metaAIActor)
	{
		// メタAIに登録
		auto metaAI = metaAIActor->GetComponent<MetaAI>();
		if (metaAI)
		{
			metaAI->RegisterTargetable(shared_from_this());
		}
	}
}
// 更新処理
void Targetable::Update(float elapsedTime)
{
    // セーフティタイマー更新
	if (_sefetyTimer > 0.0f)
	{
		_sefetyTimer -= elapsedTime;
		if (_sefetyTimer <= 0.0f)
		{
			SetTargetable(true);
			_sefetyTimer = 0.0f;
		}
    }
}
// Gui描画処理
void Targetable::DrawGui()
{
	ImGui::DragInt(u8"エリア番号", &_areaNumber, 1, -1, 1000);
	ImGui::DragFloat(u8"半径", &_radius, 0.01f, 0.0f, 100.0f);
	ImGui::DragFloat(u8"ヘイト値", &_hateValue, 0.01f, 0.0f, 100.0f);
	ImGui::Combo(u8"陣営", reinterpret_cast<int*>(&_faction), "Player\0Enemy\0Neutral\0\0");
	ImGui::Checkbox(u8"ターゲット可能か", &_isTargetable);
}
// オブジェクトとの接触時の処理
void Targetable::OnContact(CollisionData& collisionData)
{
	// ステージと接触したか確認
	auto stageController = collisionData.other->GetComponent<StageController>();
	if (stageController)
	{
		// エリア番号を更新
		_areaNumber = stageController->GetAreaNumber();
	}

    // セーフティゾーンと接触したか確認
    auto safetyZone = collisionData.other->GetComponent<SafetyZone>();
	if (safetyZone)
	{
        SetTargetable(false);
        _sefetyTimer = safetyZone->GetSafetyTimer();
    }
}
