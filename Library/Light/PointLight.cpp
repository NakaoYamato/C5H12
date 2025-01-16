#include "PointLight.h"

#include <imgui.h>

#include "../DebugSupporter/DebugSupporter.h"

// デバッグGui表示
void PointLight::DrawGui()
{
    bool useFlag = data_.isAlive == 1;
    ImGui::Checkbox(u8"使用フラグ", &useFlag);
    data_.isAlive = useFlag == true ? 1 : 0;
    ImGui::DragFloat3("position", &data_.position.x, 0.1f);
    ImGui::ColorEdit4("color", &data_.color.x);
    ImGui::DragFloat("range", &data_.range, 0.1f);
}

// デバッグ表示
void PointLight::DebugRender()
{
    Debug::Renderer::DrawSphere(data_.position.Vec3(), data_.range, data_.color);
}
