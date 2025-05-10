#include "Rigidbody.h"

#include <imgui.h>

#include "../Scene/Scene.h"

// 開始処理
void Rigidbody::Start()
{
	_orientation = Quaternion::FromRollPitchYaw(this->GetActor()->GetTransform().GetRotation());
	DirectX::XMStoreFloat4x4(&_inertiaTensor, DirectX::XMMatrixIdentity());
}

// 更新処理
void Rigidbody::Update(float elapsedTime)
{
	/// 速度更新
    UpdateVelocity(elapsedTime);
}

// 固定間隔更新処理
void Rigidbody::FixedUpdate()
{
	float deltaTime = _FIXED_UPDATE_RATE;

	/// 位置更新
    UpdatePosition(deltaTime);
}

// GUI描画
void Rigidbody::DrawGui()
{
	ImGui::DragFloat3(u8"並進速度", &_linearVelocity.x, 0.01f);
	ImGui::DragFloat3(u8"角速度", &_angularVelocity.x, 0.01f);

	ImGui::Separator();
	ImGui::DragFloat3(u8"前のフレームの位置", &_oldPosition.x, 0.01f);
	ImGui::DragFloat3(u8"前のフレームの並進速度", &_oldLinearVelocity.x, 0.01f);
	ImGui::DragFloat3(u8"前のフレームの角速度", &_oldAngularVelocity.x, 0.01f);

	ImGui::Separator();
	ImGui::DragFloat(u8"慣性質量", &_inertialMass, 0.01f);
	ImGui::DragFloat(u8"抗力係数", &_dragCoefficient, 0.01f);
	ImGui::DragFloat(u8"静止摩擦係数", &_staticFriction, 0.01f);
	ImGui::DragFloat(u8"動摩擦係数", &_dynamicFriction, 0.01f);
}

void Rigidbody::AddForce(const DirectX::XMVECTOR& force)
{
	//アキュムレータ(accumulated_force)に力(force)を累算する
	DirectX::XMStoreFloat3(&_accumulatedForce, DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&_accumulatedForce), force));
}

void Rigidbody::AddForce(const Vector3& force)
{
	AddForce(DirectX::XMLoadFloat3(&force));
}

void Rigidbody::AddTorque(const DirectX::XMVECTOR& torque)
{
	//アキュムレータ(accumulated_torque)にトルク(torque)を累算する
	DirectX::XMStoreFloat3(&_accumulatedTorque, DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&_accumulatedTorque), torque));
}

void Rigidbody::AddTorque(const Vector3& torque)
{
	AddTorque(DirectX::XMLoadFloat3(&torque));
}

DirectX::XMVECTOR Rigidbody::AddForceAtPoint(const DirectX::XMVECTOR& force, const DirectX::XMVECTOR& point)
{
	//アキュムレータ(accumulated_force)に力(force)を累算する
	DirectX::XMStoreFloat3(&_accumulatedForce, DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&_accumulatedForce), force));

	//トルク(モーメント)を計算する
	DirectX::XMVECTOR torque = {};
	torque = DirectX::XMVector3Cross(DirectX::XMVectorSubtract(point, DirectX::XMLoadFloat3(&this->GetActor()->GetTransform().GetPosition())), force);

	//アキュムレータ(accumulated_torque)にトルク(torque)を累算する
	DirectX::XMStoreFloat3(&_accumulatedTorque, DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&_accumulatedTorque), torque));

	return torque;
}

/// 速度更新
void Rigidbody::UpdateVelocity(float deltaTime)
{
	//力(_accumulatedForce)から加速度(linearAcceleration)を算出し速度(linearAcceleration)を更新する
	_oldLinearVelocity = _linearVelocity;
	DirectX::XMVECTOR linearAcceleration = {};
	linearAcceleration = DirectX::XMVectorScale(DirectX::XMLoadFloat3(&_accumulatedForce), 1 / _inertialMass);
	DirectX::XMStoreFloat3(&_linearVelocity,
		DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&_linearVelocity), DirectX::XMVectorScale(linearAcceleration, deltaTime)));

	// 速度が最低速度量より低ければ停止
	//if (DirectX::XMVectorGetX(DirectX::XMVector3Length(DirectX::XMLoadFloat3(&_linearVelocity))) < _standStillVelocity)
	//{
	//	_linearVelocity = {};
	//	// 停止フラグをオン
	//	_isStandstill = true;
	//}
	//else
	//{
	//	// 停止フラグをオフ
	//	_isStandstill = false;
	//}

	//トルク(_accumulatedTorque)から角加速度(angularAcceleration)を算出し角速度(_angularVelocity)を更新する
	_oldAngularVelocity = _angularVelocity;
	DirectX::XMVECTOR angularAcceleration;
	angularAcceleration = DirectX::XMVector3TransformCoord(DirectX::XMLoadFloat3(&_accumulatedTorque), InverseInertiaTensor(true));
	DirectX::XMStoreFloat3(&_angularVelocity, DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&_angularVelocity), DirectX::XMVectorScale(angularAcceleration, deltaTime)));

	//力のアキュムレータをゼロリセットする
	_accumulatedForce = {};

	//トルクのアキュムレータをゼロリセットする
	_accumulatedTorque = {};
}

/// 位置更新
void Rigidbody::UpdatePosition(float deltaTime)
{
	// 不動オブジェクトなら処理しない
	if (IsMovable() == false)
	{
		//ForcedStop();
		return;
	}

	if (_inertialMass <= 0.0f)
	{
		return;
	}

	// 空気抗力計算
	{
		float currentVelocityLenSq = Vector3::LengthSq(_linearVelocity);
		float dragFactor = 0.5f * _dragCoefficient * currentVelocityLenSq;
		Vector3  dragForce = Vector3::Normalize(_linearVelocity) * -dragFactor;
		AddForce(dragForce);
	}

	//並進速度による位置の更新
	_oldPosition = this->GetActor()->GetTransform().GetPosition();
	Vector3 positon = _oldPosition;
	DirectX::XMStoreFloat3(&positon,
		DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&positon),
			DirectX::XMVectorScale(DirectX::XMLoadFloat3(&_linearVelocity), deltaTime)));
	this->GetActor()->GetTransform().SetPosition(positon);

	//角速度による姿勢の更新
	// 非回転オブジェクトなら処理しない
	if (IsRotatable())
	{
		DirectX::XMVECTOR wt = DirectX::XMVectorScale(DirectX::XMLoadFloat3(&_angularVelocity), deltaTime);
		float angle = DirectX::XMVectorGetX(DirectX::XMVector3Length(wt));
		wt = DirectX::XMVector3Normalize(wt);
		DirectX::XMVECTOR q = DirectX::XMVectorScale(wt, sinf(angle * 0.5f));
		q.m128_f32[3] = cosf(angle * 0.5f);

		_orientation = Quaternion::FromRollPitchYaw(this->GetActor()->GetTransform().GetRotation());
		DirectX::XMStoreFloat4(&_orientation, DirectX::XMQuaternionMultiply(DirectX::XMLoadFloat4(&_orientation), q));
		this->GetActor()->GetTransform().SetRotation(Quaternion::ToRollPitchYaw(_orientation));
	}
}
