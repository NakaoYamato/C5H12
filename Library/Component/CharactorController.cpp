#include "CharactorController.h"

#include <imgui.h>

void CharactorController::FixedUpdate()
{
}

void CharactorController::DrawGui()
{
    ImGui::DragFloat3(u8"‰Á‘¬“x", &acceleration.x, 0.01f);
    ImGui::DragFloat3(u8"‘¬“x", &velocity.x, 0.01f);
    ImGui::Separator();
}
