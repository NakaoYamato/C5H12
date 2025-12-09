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
    {
        Parent = DirectX::XMLoadFloat4x4(parent);
		_parent = *parent;
	}
	else
	{
		_parent = Matrix4X4::Identity;
	}
    M = C * M * Parent;

	DirectX::XMStoreFloat4x4(&_transform, M);

	// ワールド座標計算
    DirectX::XMVECTOR S, R, T;
    DirectX::XMMatrixDecompose(&S, &R, &T, M);
	DirectX::XMStoreFloat3(&_worldPosition, T);
	DirectX::XMStoreFloat3(&_worldScale, S);
    _worldAngle = Quaternion::ToRollPitchYaw(R);
}

void Transform::DrawGui()
{
    ImGui::DragFloat(u8"長さの単位(m)", &_lengthScale, 0.01f, 0.01f, 10.0f);

    ImGui::DragFloat3("position", &_position.x, 0.1f);
    ImGui::DragFloat3("scale", &_scale.x, 0.1f);
    Vector3 degree = Vector3::ToDegrees(_angle);
    ImGui::DragFloat3("angle", &degree.x);
    _angle = Vector3::ToRadians(degree);

    ImGui::DragFloat3("world position", &_worldPosition.x, 0.1f);
	ImGui::DragFloat3("world scale", &_worldScale.x, 0.1f);
    degree = Vector3::ToDegrees(_worldAngle);
	ImGui::DragFloat3("world angle", &degree.x);
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

// ワールド座標設定(高負荷)
void Transform::SetWorldPosition(const Vector3& v)
{
	// 親の逆行列を取得
    Matrix4X4 parentInverse = _parent.Inverse();
	// ワールド座標をローカル座標に変換
	Vector3 localPosition = v.TransformCoord(parentInverse);
	SetPosition(localPosition);
}

// ワールドスケール設定(高負荷)
void Transform::SetWorldScale(const Vector3& v)
{
	// 親の逆行列を取得
	Matrix4X4 parentInverse = _parent.Inverse();
	// ワールドスケールをローカル座標に変換
	Vector3 localScale = v.TransformCoord(parentInverse);
	SetScale(localScale);
}

// ワールド回転設定(高負荷)
void Transform::SetWorldAngle(const Vector3& v)
{
	// 親の逆行列を取得
	Matrix4X4 parentInverse = _parent.Inverse();
	// ワールド回転をローカル座標に変換
	Vector3 localAngle = v.TransformCoord(parentInverse);
	SetAngle(localAngle);
}
