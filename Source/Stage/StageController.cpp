#include "StageController.h"

#include <imgui.h>

// GUI•`‰æ
void StageController::DrawGui()
{
	ImGui::DragInt(u8"ƒGƒŠƒA”Ô†", &_areaNumber, 1, -1, 1000);
}
