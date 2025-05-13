#include "ColliderBase.h"

#include <imgui.h>
// GUI描画
void ColliderBase::DrawGui()
{
	ImGui::Checkbox(u8"有効", &_isActive);
	ImGui::Checkbox(u8"トリガー", &_isTrigger);
}

/// 接触の解消処理
void ColliderBase::Resolve(Actor* other, 
	const Vector3& hitPosition, 
	const Vector3& hitNormal,
	const float& penetration)
{   
    // トリガーでなければ押し出し処理
    if (!_isTrigger)
    {
        // 自身のトランスフォームを取得
        Transform& transform = GetActor()->GetTransform();
        transform.SetPosition(transform.GetPosition() + hitNormal * penetration);
    }
    // アクターに接触処理を通知
    GetActor()->Contact(
        other,
        hitPosition,
        hitNormal,
        penetration);
}
