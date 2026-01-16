#include "ColliderBase.h"

#include "../../External/nameof/include/nameof.hpp"
#include "../../External/magic_enum/include/magic_enum/magic_enum.hpp"
#include <imgui.h>
// GUI描画
void ColliderBase::DrawGui()
{
	if (ImGui::TreeNode(u8"レイヤー"))
	{
		for (auto layer : magic_enum::enum_values<CollisionLayer>())
		{
			bool isChecked = (static_cast<unsigned int>(_layer) & static_cast<unsigned int>(layer));
			if (ImGui::Checkbox(magic_enum::enum_name(layer).data(), &isChecked))
			{
				unsigned int layerValue = static_cast<unsigned int>(layer);
				if (isChecked)
				{
                    _layer = static_cast<CollisionLayer>(static_cast<unsigned int>(_layer) | layerValue);
				}
				else
				{
                    _layer = static_cast<CollisionLayer>(static_cast<unsigned int>(_layer) & ~layerValue);
				}
			}
		}
        ImGui::TreePop();
	}
	if (ImGui::TreeNode(u8"レイヤーマスク"))
	{
		for (auto layer : magic_enum::enum_values<CollisionLayer>())
		{
			bool isChecked = (_layerMask & static_cast<unsigned int>(layer)) != 0;
			if (ImGui::Checkbox(magic_enum::enum_name(layer).data(), &isChecked))
			{
				unsigned int layerValue = static_cast<unsigned int>(layer);
				if (isChecked)
				{
					_layerMask |= layerValue;
				}
				else
				{
					_layerMask &= ~layerValue;
				}
			}
		}
		ImGui::TreePop();
    }
	ImGui::Checkbox(u8"有効", &_isActive);
	ImGui::Checkbox(u8"トリガー", &_isTrigger);
}

/// 接触の解消処理
void ColliderBase::OnContact(CollisionData& collisionData)
{   
}

void ColliderBase::LoadFromFile(nlohmann::json* json)
{
	// 各データ読み込み
	SetLayer(json->value("layer", CollisionLayer::None));
	SetLayerMask(json->value("layerMask", CollisionLayerMaskAll));
	SetActive(json->value("isActive", true));
	SetTrigger(json->value("isTrigger", false));
	// 派生クラスの読み込み
	OnLoad(json);
}

void ColliderBase::SaveToFile(nlohmann::json* json)
{
	(*json)["layer"]		= GetLayer();
	(*json)["layerMask"]	= GetLayerMask();
	(*json)["isActive"]		= IsActive();
	(*json)["isTrigger"]	= IsTrigger();
	// 派生クラスの保存
	OnSave(json);
}
