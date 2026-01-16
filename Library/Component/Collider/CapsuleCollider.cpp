#include "CapsuleCollider.h"

#include <imgui.h>

#include "../../Scene/Scene.h"

// 開始処理
void CapsuleCollider::Start()
{
	// コライダーの登録
	GetActor()->GetScene()->GetCollisionManager().RegisterCapsuleCollider(this);
}

// デバッグ描画処理
void CapsuleCollider::DebugRender(const RenderContext& rc)
{
	// 有効でなければ描画しない
	if (!IsActive())
		return;

	Debug::Renderer::DrawCapsule(
		GetStart().TransformCoord(GetActor()->GetTransform().GetMatrix()),
		GetEnd().TransformCoord(GetActor()->GetTransform().GetMatrix()),
		GetRadius(),
		IsTrigger() ? Vector4::Blue : Vector4::Green);
}

// 削除時処理
void CapsuleCollider::OnDelete()
{
	// コライダーの削除
	GetActor()->GetScene()->GetCollisionManager().UnregisterCapsuleCollider(this);
}

// GUI描画
void CapsuleCollider::DrawGui()
{
	ColliderBase::DrawGui();
	ImGui::DragFloat3(u8"開始座標", &_start.x, 0.01f);
	ImGui::DragFloat3(u8"終了座標", &_end.x, 0.01f);
	ImGui::DragFloat(u8"半径", &_radius, 0.01f);
}

#pragma region 入出力
// ファイル読み込み
void CapsuleCollider::OnLoad(nlohmann::json* json)
{
	// 各データ読み込み
	_start = json->value("start", Vector3(0.0f, 0.0f, 0.0f));
	_end = json->value("end", Vector3(0.0f, 1.0f, 0.0f));
	_radius = json->value("radius", 1.0f);
}

// ファイル保存
void CapsuleCollider::OnSave(nlohmann::json* json)
{
	(*json)["start"]		= _start;
	(*json)["end"]			= _end;
	(*json)["radius"]		= _radius;
}
#pragma endregion
