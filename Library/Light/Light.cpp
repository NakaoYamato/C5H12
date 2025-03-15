#include "Light.h"

#include <string>
#include <imgui.h>
#include <ImGuizmo.h>

#include "../Camera/Camera.h"
#include "../DebugSupporter/DebugSupporter.h"

// デバッグGui表示
void Light::DrawGui()
{
    static const char* typeName[] =
    {
        u8"始点",
        u8"終点",
        u8"使用しない"
    };

    ImGui::Combo(u8"ギズモ", &_currentIndex, typeName, _countof(typeName));
    if (_currentIndex == 0)
    {
        // 光の始点をギズモで動かす
        DirectX::XMFLOAT4X4 transform{};
        DirectX::XMStoreFloat4x4(&transform,
            DirectX::XMMatrixTranslation(_lightStart.x, _lightStart.y, _lightStart.z));
        const DirectX::XMFLOAT4X4& view = Camera::Instance().GetView();
        const DirectX::XMFLOAT4X4& projection = Camera::Instance().GetProjection();
        if (ImGuizmo::Manipulate(
            &view._11, &projection._11,
            ImGuizmo::TRANSLATE,
            ImGuizmo::WORLD,
            &transform._11,
            nullptr))
        {
            _lightStart.x = transform._41;
            _lightStart.y = transform._42;
            _lightStart.z = transform._43;

            // 光の向きの計算
            _direction = Vec3Normalize(_lightEnd - _lightStart);
        }
    }
    else if (_currentIndex == 1)
    {
        // 光の終点をギズモで動かす
        DirectX::XMFLOAT4X4 transform{};
        DirectX::XMStoreFloat4x4(&transform,
            DirectX::XMMatrixTranslation(_lightEnd.x, _lightEnd.y, _lightEnd.z));
        const DirectX::XMFLOAT4X4& view = Camera::Instance().GetView();
        const DirectX::XMFLOAT4X4& projection = Camera::Instance().GetProjection();
        if (ImGuizmo::Manipulate(
            &view._11, &projection._11,
            ImGuizmo::TRANSLATE,
            ImGuizmo::WORLD,
            &transform._11,
            nullptr))
        {
            _lightEnd.x = transform._41;
            _lightEnd.y = transform._42;
            _lightEnd.z = transform._43;

            // 光の向きの計算
            _direction = Vec3Normalize(_lightEnd - _lightStart);
        }
    }

    if (ImGui::DragFloat4("Direction", &_direction.x, 0.1f))
    {
        // 正規化
        Vector3 d = Vector3(_direction.x, _direction.y, _direction.z);
        if (Vec3LengthSq(d) == 0.0f)
        {
            _direction = {};
        }
        else
        {
            d = Vec3Normalize(d);
            _direction.x = d.x;
            _direction.y = d.y;
            _direction.z = d.z;
            _direction.w = 0.0f;
        }
    }

    ImGui::ColorEdit4("Color", &_color.x);
    ImGui::ColorEdit4("AmbientColor", &_ambientColor.x);
}

// デバッグ表示
void Light::DebugRender()
{
    Debug::Renderer::DrawSphere(_lightStart, 0.5f, { 1,0,0,1 });
    Debug::Renderer::DrawSphere(_lightEnd, 0.5f, { 0,1,0,1 });
    Debug::Renderer::DrawArrow(_lightStart, _lightEnd, 1.0f, { 1,1,1,1 });
}
