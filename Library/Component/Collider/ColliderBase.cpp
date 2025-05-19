#include "ColliderBase.h"

#include "../../External/nameof/include/nameof.hpp"
#include "../../External/magic_enum/include/magic_enum/magic_enum.hpp"
#include <imgui.h>
// GUI描画
void ColliderBase::DrawGui()
{
    std::vector<const char*> layerNames;
	for (auto layer : magic_enum::enum_values<CollisionLayer>())
	{
		layerNames.push_back(magic_enum::enum_name(layer).data());
	}
	int layer = static_cast<int>(_layer);
	ImGui::Combo(u8"レイヤー", &layer, layerNames.data(), static_cast<int>(layerNames.size()));
	_layer = static_cast<CollisionLayer>(layer);
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
