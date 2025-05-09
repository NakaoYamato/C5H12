#include "CapsuleCollider.h"

#include <imgui.h>

#include "../../Scene/Scene.h"

// 開始処理
void CapsuleCollider::Start()
{
	// コライダーの登録
	GetActor()->GetScene()->GetCollisionManager().RegisterCapsuleCollider(this);
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
	ImGui::DragFloat3(u8"開始座標", &_start.x, 0.01f);
	ImGui::DragFloat3(u8"終了座標", &_end.x, 0.01f);
	ImGui::DragFloat(u8"半径", &_radius, 0.01f);
}
