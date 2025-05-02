#include "Transform.h"

#include <imgui.h>

void Transform::UpdateTransform(const DirectX::XMFLOAT4X4* parent)
{
    DirectX::XMMATRIX C{ DirectX::XMMatrixScaling(_lengthScale, _lengthScale,_lengthScale) };

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
    ImGui::DragFloat(u8"’·‚³‚Ì’PˆÊ(m)", &_lengthScale, 0.01f, 0.01f, 10.0f);

    ImGui::DragFloat3("position", &_position.x, 0.1f);
    ImGui::DragFloat3("scale", &_scale.x, 0.1f);
    Vector3 degree = Vec3ConvertToDegrees(_angle);
    ImGui::DragFloat3("angle", &degree.x);
    _angle = Vec3ConvertToRadians(degree);
    Vector3 wp = GetWorldPosition();
    ImGui::DragFloat3("world position", &wp.x, 0.1f);
}
