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
    DirectX::XMMATRIX C{ DirectX::XMLoadFloat4x4(&COORDINATE_SYSTEM_TRANSFORMS[_coordinateType]) *
        DirectX::XMMatrixScaling(_lengthScale, _lengthScale,_lengthScale) };

	DirectX::XMMATRIX T = DirectX::XMMatrixTranslationFromVector(DirectX::XMLoadFloat3(&_position));
    DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(_angle.x, _angle.y, _angle.z);
    DirectX::XMMATRIX S = DirectX::XMMatrixScalingFromVector(DirectX::XMLoadFloat3(&_scale));

    DirectX::XMMATRIX Parent = DirectX::XMMatrixIdentity();
    if (parent != nullptr)
        Parent = DirectX::XMLoadFloat4x4(parent);

	DirectX::XMStoreFloat4x4(&_transform,
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
    int type = static_cast<int>(_coordinateType);
    if (ImGui::Combo(u8"座標系", &type, coordinates, 4))
        _coordinateType = static_cast<COORDINATE_TYPE>(type);
    ImGui::DragFloat(u8"長さの単位(m)", &_lengthScale, 0.01f, 0.01f, 10.0f);

    ImGui::DragFloat3("position", &_position.x, 0.1f);
    ImGui::DragFloat3("scale", &_scale.x, 0.1f);
    Vector3 degree = Vec3ConvertToDegrees(_angle);
    ImGui::DragFloat(u8"X軸", &degree.x);
    ImGui::DragFloat(u8"Y軸", &degree.y);
    ImGui::DragFloat(u8"Z軸", &degree.z);
    _angle = Vec3ConvertToRadians(degree);
    Vector3 wp = GetWorldPosition();
    ImGui::DragFloat3("world position", &wp.x, 0.1f);
}
