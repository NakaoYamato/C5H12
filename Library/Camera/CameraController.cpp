#include "CameraController.h"
#include "Camera.h"

#include <imgui.h>

// デバッグ用Gui
void CameraControllerBase::DrawGui()
{
    ImGui::DragFloat3("target", &_target.x);
    ImGui::DragFloat3("eye", &_eye.x);
    Vector3 deg = Vector3::ToDegrees(_angle);
    if (ImGui::DragFloat3("angle", &deg.x))
    {
        _angle = Vector3::ToRadians(deg);
    }
}
