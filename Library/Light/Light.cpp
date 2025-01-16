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

    ImGui::Combo(u8"ギズモ", &currentIndex_, typeName, _countof(typeName));
    if (currentIndex_ == 0)
    {
        // 光の始点をギズモで動かす
        DirectX::XMFLOAT4X4 transform{};
        DirectX::XMStoreFloat4x4(&transform,
            DirectX::XMMatrixTranslation(lightStart_.x, lightStart_.y, lightStart_.z));
        const DirectX::XMFLOAT4X4& view = Camera::Instance().GetView();
        const DirectX::XMFLOAT4X4& projection = Camera::Instance().GetProjection();
        if (ImGuizmo::Manipulate(
            &view._11, &projection._11,
            ImGuizmo::TRANSLATE,
            ImGuizmo::WORLD,
            &transform._11,
            nullptr))
        {
            lightStart_.x = transform._41;
            lightStart_.y = transform._42;
            lightStart_.z = transform._43;

            // 光の向きの計算
            direction_ = Vec3Normalize(lightEnd_ - lightStart_);
        }
    }
    else if (currentIndex_ == 1)
    {
        // 光の終点をギズモで動かす
        DirectX::XMFLOAT4X4 transform{};
        DirectX::XMStoreFloat4x4(&transform,
            DirectX::XMMatrixTranslation(lightEnd_.x, lightEnd_.y, lightEnd_.z));
        const DirectX::XMFLOAT4X4& view = Camera::Instance().GetView();
        const DirectX::XMFLOAT4X4& projection = Camera::Instance().GetProjection();
        if (ImGuizmo::Manipulate(
            &view._11, &projection._11,
            ImGuizmo::TRANSLATE,
            ImGuizmo::WORLD,
            &transform._11,
            nullptr))
        {
            lightEnd_.x = transform._41;
            lightEnd_.y = transform._42;
            lightEnd_.z = transform._43;

            // 光の向きの計算
            direction_ = Vec3Normalize(lightEnd_ - lightStart_);
        }
    }

    if (ImGui::DragFloat4("Direction", &direction_.x, 0.1f))
    {
        // 正規化
        Vector3 d = Vector3(direction_.x, direction_.y, direction_.z);
        if (Vec3LengthSq(d) == 0.0f)
        {
            direction_ = {};
        }
        else
        {
            d = Vec3Normalize(d);
            direction_.x = d.x;
            direction_.y = d.y;
            direction_.z = d.z;
            direction_.w = 0.0f;
        }
    }

    ImGui::ColorEdit4("Color", &color_.x);
    ImGui::ColorEdit4("AmbientColor", &ambientColor_.x);
}

// デバッグ表示
void Light::DebugRender()
{
    Debug::Renderer::DrawSphere(lightStart_, 0.5f, { 1,0,0,1 });
    Debug::Renderer::DrawSphere(lightEnd_, 0.5f, { 0,1,0,1 });
    Debug::Renderer::DrawArrow(lightStart_, lightEnd_, 1.0f, { 1,1,1,1 });
}
