#include "Rigidbody.h"

#include <imgui.h>

// 開始処理
void Rigidbody::Start()
{
	_orientation = QuaternionFromRollPitchYaw(this->GetActor()->GetTransform().GetRotation());
	DirectX::XMStoreFloat4x4(&_inertiaTensor, DirectX::XMMatrixIdentity());
}

// 更新処理
void Rigidbody::Update(float elapsedTime)
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

	//力(accumulated_force)から加速度(linear_acceleration)を算出し速度(linear_velocity)を更新する
	_oldLinearVelocity = _linearVelocity;
	DirectX::XMVECTOR linearAcceleration = {};
	linearAcceleration = DirectX::XMVectorScale(DirectX::XMLoadFloat3(&_accumulatedForce), 1 / _inertialMass);
	DirectX::XMStoreFloat3(&_linearVelocity, 
		DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&_linearVelocity), DirectX::XMVectorScale(linearAcceleration, elapsedTime)));
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

	//並進速度による位置の更新
	_oldPosition = this->GetActor()->GetTransform().GetPosition();
	Vector3 positon = _oldPosition;
	DirectX::XMStoreFloat3(&positon,
		DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&positon),
			DirectX::XMVectorScale(DirectX::XMLoadFloat3(&_linearVelocity), elapsedTime)));
	this->GetActor()->GetTransform().SetPosition(positon);

	//トルク(accumulated_torque)から角加速度(angular_acceleration)を算出し角速度(angular_velocity)を更新する
	_oldAngularVelocity = _angularVelocity;
	DirectX::XMVECTOR angularAcceleration;
	angularAcceleration = DirectX::XMVector3TransformCoord(DirectX::XMLoadFloat3(&_accumulatedTorque), InverseInertiaTensor(true));
	DirectX::XMStoreFloat3(&_angularVelocity, DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&_angularVelocity), DirectX::XMVectorScale(angularAcceleration, elapsedTime)));

	//角速度による姿勢の更新
	// 非回転オブジェクトなら処理しない
	if (IsRotatable())
	{
		DirectX::XMVECTOR wt = DirectX::XMVectorScale(DirectX::XMLoadFloat3(&_angularVelocity), elapsedTime);
		float angle = DirectX::XMVectorGetX(DirectX::XMVector3Length(wt));
		wt = DirectX::XMVector3Normalize(wt);
		DirectX::XMVECTOR q = DirectX::XMVectorScale(wt, sinf(angle * 0.5f));
		q.m128_f32[3] = cosf(angle * 0.5f);

		_orientation = QuaternionFromRollPitchYaw(this->GetActor()->GetTransform().GetRotation());
		DirectX::XMStoreFloat4(&_orientation, DirectX::XMQuaternionMultiply(DirectX::XMLoadFloat4(&_orientation), q));
		this->GetActor()->GetTransform().SetAngle(QuaternionToRollPitchYaw(_orientation));
	}

	//力のアキュムレータをゼロリセットする
	_accumulatedForce = {};

	//トルクのアキュムレータをゼロリセットする
	_accumulatedTorque = {};
}

// GUI描画
void Rigidbody::DrawGui()
{
	ImGui::DragFloat3("linear velocity", &_linearVelocity.x, 0.01f);
	ImGui::DragFloat3("angular velocity", &_angularVelocity.x, 0.01f);

	ImGui::Separator();
	ImGui::DragFloat3("old position", &_oldPosition.x, 0.01f);
	ImGui::DragFloat3("old linear velocity", &_oldLinearVelocity.x, 0.01f);
	ImGui::DragFloat3("old angular velocity", &_oldAngularVelocity.x, 0.01f);

	ImGui::Separator();
	ImGui::DragFloat("mass", &_inertialMass, 0.01f);
	ImGui::DragFloat("static friction", &_staticFriction, 0.01f);
	ImGui::DragFloat("dynamic friction", &_dynamicFriction, 0.01f);
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
