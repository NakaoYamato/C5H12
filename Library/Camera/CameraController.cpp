#include "CameraController.h"
#include "Camera.h"

#include <imgui.h>

// デバッグ用Gui
void CameraControllerBase::DrawGui()
{
    ImGui::DragFloat3("target", &target_.x);
    ImGui::DragFloat3("eye", &eye_.x);
    Vector3 deg = Vec3ConvertToDegrees(angle_);
    if (ImGui::DragFloat3("angle", &deg.x))
    {
        angle_ = Vec3ConvertToRadians(deg);
    }
}
