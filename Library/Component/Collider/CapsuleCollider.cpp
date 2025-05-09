#include "CapsuleCollider.h"

#include <imgui.h>

#include "../../Scene/Scene.h"
#include "../../DebugSupporter/DebugSupporter.h"

// 開始処理
void CapsuleCollider::Start()
{
	// コライダーの登録
	GetActor()->GetScene()->GetCollisionManager().RegisterCapsuleCollider(this);
}

// デバッグ描画処理
void CapsuleCollider::DebugRender(const RenderContext& rc)
{
	// デバッグ描画フラグがオフなら何もしない
	if (!GetActor()->IsDrawingDebug())return;

	// 有効でなければ描画しない
	if (!IsActive())
		return;

	Debug::Renderer::DrawCapsule(
		GetStart().TransformCoord(GetActor()->GetTransform().GetMatrix()),
		GetEnd().TransformCoord(GetActor()->GetTransform().GetMatrix()),
		GetRadius(),
		IsTrigger() ? _VECTOR4_BLUE : _VECTOR4_GREEN);
}

// 削除時処理
void CapsuleCollider::OnDestroy()
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
