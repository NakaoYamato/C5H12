#include "CameraController.h"
#include "Camera.h"

#include <imgui.h>

// デバッグ用Gui
void CameraControllerBase::DrawGui()
{
    ImGui::DragFloat3("target", &_target.x);
    ImGui::DragFloat3("eye", &_eye.x);
    Vector3 deg = Vec3ConvertToDegrees(_angle);
    if (ImGui::DragFloat3("angle", &deg.x))
    {
        _angle = Vec3ConvertToRadians(deg);
    }
}
