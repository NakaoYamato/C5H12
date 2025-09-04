#include "ColliderBase.h"

#include "../../External/nameof/include/nameof.hpp"
#include "../../External/magic_enum/include/magic_enum/magic_enum.hpp"
#include <imgui.h>
// GUI•`‰æ
void ColliderBase::DrawGui()
{
    std::vector<const char*> layerNames;
	for (auto layer : magic_enum::enum_values<CollisionLayer>())
	{
		layerNames.push_back(magic_enum::enum_name(layer).data());
	}
	int layer = static_cast<int>(_layer);
	ImGui::Combo(u8"ƒŒƒCƒ„[", &layer, layerNames.data(), static_cast<int>(layerNames.size()));
	_layer = static_cast<CollisionLayer>(layer);
	ImGui::Checkbox(u8"—LŒø", &_isActive);
	ImGui::Checkbox(u8"ƒgƒŠƒK[", &_isTrigger);
}

/// ÚG‚Ì‰ğÁˆ—
void ColliderBase::OnContact(CollisionData& collisionData)
{   
}
