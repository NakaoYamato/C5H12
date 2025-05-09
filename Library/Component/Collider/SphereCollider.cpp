#include "SphereCollider.h"

#include <imgui.h>

#include "../../Scene/Scene.h"

void SphereCollider::Start()
{
	// コライダーの登録
	GetActor()->GetScene()->GetCollisionManager().RegisterSphereCollider(this);
}

// 削除時処理
void SphereCollider::OnDestroy()
{
	// コライダーの削除
	GetActor()->GetScene()->GetCollisionManager().UnregisterSphereCollider(this);
}

// GUI描画
void SphereCollider::DrawGui()
{
	ImGui::DragFloat3(u8"中心座標", &_center.x, 0.01f);
	ImGui::DragFloat(u8"半径", &_radius, 0.01f, 0.0f, 100.0f);
}
