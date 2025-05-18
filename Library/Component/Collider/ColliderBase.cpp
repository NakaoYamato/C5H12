#include "ColliderBase.h"

#include <imgui.h>
// GUI描画
void ColliderBase::DrawGui()
{
    ImGui::InputText(u8"レイヤー", &_layer);
	ImGui::Checkbox(u8"有効", &_isActive);
	ImGui::Checkbox(u8"トリガー", &_isTrigger);
}

/// 接触の解消処理
void ColliderBase::OnContact(CollisionData& collisionData)
{   
    // トリガーでなければ押し出し処理
    //if (!_isTrigger)
    //{
    //    // 自身のトランスフォームを取得
    //    Transform& transform = GetActor()->GetTransform();
    //    transform.SetPosition(transform.GetPosition() + collisionData.hitNormal * collisionData.penetration);
    //}
}
