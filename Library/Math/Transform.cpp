#include "Transform.h"

#include <imgui.h>

void Transform::UpdateTransform(const DirectX::XMFLOAT4X4* parent)
{
    DirectX::XMMATRIX C{ DirectX::XMMatrixScaling(_lengthScale, _lengthScale,_lengthScale) };

	DirectX::XMMATRIX M = Matrix4X4::CreateTransform(
		_scale,
		_angle,
		_position
	).ToXMMATRIX();

    DirectX::XMMATRIX Parent = DirectX::XMMatrixIdentity();
    if (parent != nullptr)
        Parent = DirectX::XMLoadFloat4x4(parent);

	DirectX::XMStoreFloat4x4(&_transform,
        C *
        M *
        Parent
	);
}

void Transform::DrawGui()
{
    ImGui::DragFloat(u8"長さの単位(m)", &_lengthScale, 0.01f, 0.01f, 10.0f);

    ImGui::DragFloat3("position", &_position.x, 0.1f);
    ImGui::DragFloat3("scale", &_scale.x, 0.1f);
    Vector3 degree = Vector3::ToDegrees(_angle);
    ImGui::DragFloat3("angle", &degree.x);
    _angle = Vector3::ToRadians(degree);
    Vector3 wp = GetWorldPosition();
    ImGui::DragFloat3("world position", &wp.x, 0.1f);
}

/// 指定方向（ワールド空間）を向く
void Transform::LookAt(const Vector3& worldDirection)
{
	Quaternion q = Quaternion::LookAt(_position, _position + worldDirection, GetAxisY());
	_angle = Quaternion::ToRollPitchYaw(q);
}

/// 値をリセット
void Transform::Reset()
{
    _lengthScale = 1.0f;
    _position = Vector3::Zero;
    _scale = Vector3::One;
    _angle = Vector3::Zero;
    _transform = Matrix4X4::Identity;
}
