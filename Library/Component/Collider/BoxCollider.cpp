#include "BoxCollider.h"

#include <imgui.h>

#include "../../Scene/Scene.h"

// 開始処理
void BoxCollider::Start()
{
	// コライダーの登録
	GetActor()->GetScene()->GetCollisionManager().RegisterBoxCollider(this);
}

// 削除時処理
void BoxCollider::OnDestroy()
{
	// コライダーの削除
	GetActor()->GetScene()->GetCollisionManager().UnregisterBoxCollider(this);
}

// 削除時処理
void BoxCollider::DrawGui()
{
	ImGui::DragFloat3(u8"中心座標", &_center.x, 0.01f);
	ImGui::DragFloat3(u8"半辺長", &_halfSize.x, 0.01f);
}
