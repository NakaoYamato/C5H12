#include "Transform.h"

#include <imgui.h>

void Transform::UpdateTransform(const DirectX::XMFLOAT4X4* parent)
{
    static const DirectX::XMFLOAT4X4 COORDINATE_SYSTEM_TRANSFORMS[]
    {
        {
            -1,0,0,0,
            0,1,0,0,
            0,0,1,0,
            0,0,0,1
        },
        {
            1,0,0,0,
            0,1,0,0,
            0,0,1,0,
            0,0,0,1
        },
        {
            -1,0,0,0,
            0,0,-1,0,
            0,1,0,0,
            0,0,0,1
        },
        {
            -1,0,0,0,
            0,0,1,0,
            0,1,0,0,
            0,0,0,1
        },
    };
    DirectX::XMMATRIX C{ DirectX::XMLoadFloat4x4(&COORDINATE_SYSTEM_TRANSFORMS[coordinateType_]) *
        DirectX::XMMatrixScaling(lengthScale_, lengthScale_,lengthScale_) };

	DirectX::XMMATRIX T = DirectX::XMMatrixTranslationFromVector(DirectX::XMLoadFloat3(&position_));
    DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(angle_.x, angle_.y, angle_.z);
    DirectX::XMMATRIX S = DirectX::XMMatrixScalingFromVector(DirectX::XMLoadFloat3(&scale_));

    DirectX::XMMATRIX Parent = DirectX::XMMatrixIdentity();
    if (parent != nullptr)
        Parent = DirectX::XMLoadFloat4x4(parent);

	DirectX::XMStoreFloat4x4(&transform,
        C *
		S *
		R *
        T *
        Parent
	);
}

void Transform::DrawGui()
{
    static const char* coordinates[4] =
    {
        u8"右手Y軸UP",
        u8"左手Y軸UP",
        u8"右手Z軸UP",
        u8"左手Z軸UP"
    };
    int type = static_cast<int>(coordinateType_);
    if (ImGui::Combo(u8"座標系", &type, coordinates, 4))
        coordinateType_ = static_cast<COORDINATE_TYPE>(type);
    ImGui::DragFloat(u8"長さの単位(m)", &lengthScale_, 0.01f, 0.01f, 10.0f);

    ImGui::DragFloat3("position", &position_.x, 0.1f);
    ImGui::DragFloat3("scale", &scale_.x, 0.1f);
    Vector3 degree = Vec3ConvertToDegrees(angle_);
    ImGui::DragFloat(u8"X軸", &degree.x);
    ImGui::DragFloat(u8"Y軸", &degree.y);
    ImGui::DragFloat(u8"Z軸", &degree.z);
    angle_ = Vec3ConvertToRadians(degree);
}
