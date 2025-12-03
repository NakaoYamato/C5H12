#include "CombatStatusController.h"

#include "../../Library/Scene/Scene.h"
#include "../../Library/Algorithm/Converter.h"

#include <imgui.h>

// 開始処理
void CombatStatusController::Start()
{	
	// メタAI取得
	auto metaAIActor = GetActor()->GetScene()->GetActorManager().FindByName("MetaAI", ActorTag::System);
	if (metaAIActor)
	{
		_metaAI = metaAIActor->GetComponent<MetaAI>();
	}

	// 現在の位置を縄張りとする
    _territoryCenter = GetActor()->GetTransform().GetWorldPosition();
}

// 更新処理
void CombatStatusController::Update(float elapsedTime)
{
	if (!_isUpdate) return;

	// 状態ごとの更新
	switch (_currentStatus)
	{
	case Status::Normal:
		UpdateNormalStatus(elapsedTime);
		break;
	case Status::Alert:
		UpdateAlertStatus(elapsedTime);
		break;
	case Status::Combat:
		UpdateCombatStatus(elapsedTime);
		break;
	default:
		break;
	}
	_statusTimer += elapsedTime;
}

// GUI描画
void CombatStatusController::DrawGui()
{
	const char* statusNames[] = { u8"通常", u8"警戒", u8"戦闘" };
	int currentStatus = static_cast<int>(_currentStatus);
	if (ImGui::Combo(u8"戦闘状態", &currentStatus, statusNames, IM_ARRAYSIZE(statusNames)))
	{
		_currentStatus = static_cast<Status>(currentStatus);
	}
	ImGui::Separator();
	ImGui::Text(u8"ターゲット陣営: %s", ToString<Targetable::Faction>(static_cast<size_t>(_targetFaction)).c_str());
	ImGui::InputFloat3(u8"ターゲット座標", &_targetPosition.x);
	ImGui::InputFloat(u8"ターゲットの半径", &_targetRadius);
	ImGui::InputFloat(u8"検索範囲", &_searchRange);
	ImGui::InputFloat(u8"戦闘継続範囲", &_combatRange);
	ImGui::Checkbox(u8"更新する", &_isUpdate);
    ImGui::DragFloat3(u8"縄張り中心座標", &_territoryCenter.x, 0.1f);
}

// 現在位置からターゲットまでのベクトル取得
Vector3 CombatStatusController::GetToTargetVec(const Vector3& target)
{
	return target - GetActor()->GetTransform().GetPosition();
}

// ターゲットの範囲内かどうか
bool CombatStatusController::IsInTargetRange(const Vector3& targetPosition, float targetRadius)
{
	return (targetPosition - GetActor()->GetTransform().GetPosition()).Length() <= targetRadius;
}

// 通常状態の更新
void CombatStatusController::UpdateNormalStatus(float elapsedTime)
{
	// メタAIからターゲット座標を取得
	if (auto metaAI = _metaAI.lock())
	{
		auto targetable = metaAI->SearchTarget(
			_targetFaction,
			GetActor()->GetTransform().GetWorldPosition(),
			_searchRange);
		// ターゲットが見つかれば警戒状態へ
		if (targetable)
		{
			SetTargetPosition(targetable->GetActor()->GetTransform().GetWorldPosition());
			SetStatus(CombatStatusController::Status::Alert);
			return;
		}
	}

	// 縄張りをターゲットにする
    SetTargetPosition(_territoryCenter);
}

// 警戒状態の更新
void CombatStatusController::UpdateAlertStatus(float elapsedTime)
{
	// メタAIからターゲット座標を取得
	if (auto metaAI = _metaAI.lock())
	{
		auto targetable = metaAI->SearchTarget(
			_targetFaction,
			GetActor()->GetTransform().GetWorldPosition(),
			_searchRange);
		if (targetable)
		{
			SetTargetPosition(targetable->GetActor()->GetTransform().GetWorldPosition());
		}
	}
}

// 戦闘状態の更新
void CombatStatusController::UpdateCombatStatus(float elapsedTime)
{
	// メタAIからターゲット座標を取得
	if (auto metaAI = _metaAI.lock())
	{
		auto targetable = metaAI->SearchTarget(
			_targetFaction,
			GetActor()->GetTransform().GetWorldPosition(),
			_combatRange);
		if (targetable)
		{
			SetTargetPosition(targetable->GetActor()->GetTransform().GetWorldPosition());
		}
		else
		{
			// ターゲットが見つからなければ警戒状態へ
			SetStatus(CombatStatusController::Status::Normal);
		}
	}
}
