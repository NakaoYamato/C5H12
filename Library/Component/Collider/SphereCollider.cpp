#include "SphereCollider.h"

#include <imgui.h>

#include "../../Scene/Scene.h"
#include "../../DebugSupporter/DebugSupporter.h"

void SphereCollider::Start()
{
	// コライダーの登録
	GetActor()->GetScene()->GetCollisionManager().RegisterSphereCollider(this);
}

// デバッグ描画処理
void SphereCollider::DebugRender(const RenderContext& rc)
{
	// 有効でなければ描画しない
	if (!IsActive())
		return;

	Debug::Renderer::DrawSphere(
		GetPosition().TransformCoord(GetActor()->GetTransform().GetMatrix()),
		GetRadius(),
		IsTrigger() ? Vector4::Blue : Vector4::Green);
}

// 削除時処理
void SphereCollider::OnDelete()
{
	// コライダーの削除
	GetActor()->GetScene()->GetCollisionManager().UnregisterSphereCollider(this);
}

// GUI描画
void SphereCollider::DrawGui()
{
	ColliderBase::DrawGui();
	ImGui::DragFloat3(u8"中心座標", &_center.x, 0.01f);
	ImGui::DragFloat(u8"半径", &_radius, 0.01f, 0.0f, 100.0f);
}
