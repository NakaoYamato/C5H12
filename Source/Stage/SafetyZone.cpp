#include "SafetyZone.h"

#include <imgui.h>

// GUI描画
void SafetyZone::DrawGui()
{
    ImGui::DragFloat(u8"セーフティタイマー", &_sefetyTimer, 0.01f, 0.0f, 10.0f);
}
