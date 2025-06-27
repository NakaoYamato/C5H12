#include "WyvernController.h"

#include "../../Source/Common/Damageable.h"
#include "../EnemyController.h"

#include <imgui.h>

// –¼‘Oæ“¾
void WyvernController::Start()
{
}
// GUI•`‰æ
void WyvernController::DrawGui()
{
	ImGui::DragFloat(u8"‹ßÚUŒ‚”ÍˆÍ", &_nearAttackRange, 0.1f, 0.0f, 100.0f, "%.1f m");
	ImGui::DragFloat(u8"‹ßÚUŒ‚Šp“x", &_nearAttackRadian, 0.01f, 0.0f, DirectX::XM_PI, "%.1f rad");
	ImGui::DragFloat(u8"‰ñ“]‘¬“x", &_rotationSpeed, 0.1f, 0.0f, 10.0f, "%.1f");
	ImGui::DragFloat(u8"ƒ^[ƒQƒbƒg‚ÉŒü‚­Šp“x", &_lookAtRadian, 0.01f, 0.0f, DirectX::XM_PI, "%.1f rad");
}
