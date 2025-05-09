#include "ColliderBase.h"

#include <imgui.h>
// GUI•`‰æ
void ColliderBase::DrawGui()
{
	ImGui::Checkbox(u8"—LŒø", &_isActive);
	ImGui::Checkbox(u8"ƒgƒŠƒK[", &_isTrigger);
}
