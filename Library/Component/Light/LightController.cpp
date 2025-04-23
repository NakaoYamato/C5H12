#include "LightController.h"

#include "../../DebugSupporter/DebugSupporter.h"
#include "../../Camera/Camera.h"

#include <imgui.h>
#include <ImGuizmo.h>

// デバッグ表示
void LightController::DebugRender(const RenderContext& rc)
{
	DirectX::XMFLOAT4X4 transform = GetActor()->GetTransform().GetMatrix();
    const DirectX::XMFLOAT4X4& view = Camera::Instance().GetView();
    const DirectX::XMFLOAT4X4& projection = Camera::Instance().GetProjection();
    if (Debug::Guizmo(view, projection, &transform, ImGuizmo::OPERATION::ROTATE))
    {
        DirectX::XMMATRIX M = DirectX::XMLoadFloat4x4(&transform);
        DirectX::XMVECTOR S, R, T;
        DirectX::XMMatrixDecompose(&S, &R, &T, M);
        Vector3 s, r, t;
        r = QuaternionToRollPitchYaw(R);
        GetActor()->GetTransform().SetAngle(r);
    }

	Debug::Renderer::DrawAxis(transform);
}

// GUI描画
void LightController::DrawGui()
{
    ImGui::ColorEdit4("Color", &_color.x);
    ImGui::ColorEdit4("AmbientColor", &_ambientColor.x);
}
