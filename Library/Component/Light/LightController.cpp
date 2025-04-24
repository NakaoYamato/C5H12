#include "LightController.h"

#include "../../DebugSupporter/DebugSupporter.h"
#include "../../Scene/Scene.h"

#include <imgui.h>
#include <ImGuizmo.h>

// デバッグ表示
void LightController::DebugRender(const RenderContext& rc)
{
    DirectX::XMFLOAT4X4 transform = GetActor()->GetTransform().GetMatrix();
    // ヒエラルキーに表示中のみ描画
    if (GetActor()->DrawHierarchy())
    {
        const DirectX::XMFLOAT4X4& view = GetActor()->GetScene()->GetMainCamera()->GetView();
        const DirectX::XMFLOAT4X4& projection = GetActor()->GetScene()->GetMainCamera()->GetProjection();
        if (Debug::Guizmo(view, projection, &transform, ImGuizmo::OPERATION::ROTATE, ImGuizmo::MODE::LOCAL))
        {
            DirectX::XMMATRIX M = DirectX::XMLoadFloat4x4(&transform);
            DirectX::XMVECTOR S, R, T;
            DirectX::XMMatrixDecompose(&S, &R, &T, M);
            Vector3 s, r, t;
            r = QuaternionToRollPitchYaw(R);
            GetActor()->GetTransform().SetAngle(r);
        }

    }
    Debug::Renderer::DrawAxis(transform);
}

// GUI描画
void LightController::DrawGui()
{
    ImGui::ColorEdit4("Color", &_color.x);
    ImGui::ColorEdit4("AmbientColor", &_ambientColor.x);
}
