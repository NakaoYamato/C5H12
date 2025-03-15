#include "PointLight.h"

#include <imgui.h>

#include "../DebugSupporter/DebugSupporter.h"

// デバッグGui表示
void PointLight::DrawGui()
{
    bool useFlag = _data.isAlive == 1;
    ImGui::Checkbox(u8"使用フラグ", &useFlag);
    _data.isAlive = useFlag == true ? 1 : 0;
    ImGui::DragFloat3("position", &_data.position.x, 0.1f);
    ImGui::ColorEdit4("color", &_data.color.x);
    ImGui::DragFloat("range", &_data.range, 0.1f);
}

// デバッグ表示
void PointLight::DebugRender()
{
    Debug::Renderer::DrawSphere(_data.position.Vec3(), _data.range, _data.color);
}
