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
// 接触の解消処理
void Rigidbody::OnContact(CollisionData& collisionData)
{
	// トリガーでなければ押し出し処理
	if (!collisionData.isTrigger && !collisionData.otherIsTrigger)
	{
		ContactResolve(collisionData);
	}
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
		this->GetActor()->GetTransform().SetAngle(Quaternion::ToRollPitchYaw(_orientation));
	}
}
/// 接触の解消処理
void Rigidbody::ContactResolve(CollisionData& collisionData)
{
	// 自身のRigidbody
	auto body0 = this;

	// 衝突しているオブジェクトのRigidbodyを取得
	auto body1 = collisionData.other->GetComponent<Rigidbody>();
	if (body1 == nullptr)
		return;

	float vrel = 0;
	DirectX::XMVECTOR normalVec = DirectX::XMLoadFloat3(&collisionData.hitNormal);

	// ra	: オブジェクト0から接触点までの距離
	// rb	: オブジェクト1から接触点までの距離
	DirectX::XMVECTOR ra, rb;
	DirectX::XMVECTOR pointVec = DirectX::XMLoadFloat3(&collisionData.hitPosition);

	ra = DirectX::XMVectorSubtract(pointVec, DirectX::XMLoadFloat3(&body0->_oldPosition));
	rb = DirectX::XMVectorSubtract(pointVec, DirectX::XMLoadFloat3(&body1->_oldPosition));

	//(8-1)
	DirectX::XMVECTOR pdota;
	pdota = DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&body0->_oldLinearVelocity),
		DirectX::XMVector3Cross(DirectX::XMLoadFloat3(&body0->_oldAngularVelocity),
			ra));

	//(8-2)
	DirectX::XMVECTOR pdotb;
	pdotb = DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&body1->_oldLinearVelocity),
		DirectX::XMVector3Cross(DirectX::XMLoadFloat3(&body1->_oldAngularVelocity),
			rb));

	//(8-3)
	vrel = DirectX::XMVectorGetX(
		DirectX::XMVector3Dot(normalVec,
			DirectX::XMVectorSubtract(pdota, pdotb)));

	float numerator = 0;
	numerator = -(1.0f + body0->_restitution) * vrel;

	float denominator = 0;
	float term1 = body0->InverseMass();
	float term2 = body1->InverseMass();

	DirectX::XMVECTOR ta, tb;
	ta = DirectX::XMVector3Dot(normalVec,
		DirectX::XMVector3Cross(
			DirectX::XMVector3TransformCoord(DirectX::XMVector3Cross(ra, normalVec), body0->InverseInertiaTensor()),
			ra));
	tb = DirectX::XMVector3Dot(normalVec,
		DirectX::XMVector3Cross(
			DirectX::XMVector3TransformCoord(DirectX::XMVector3Cross(rb, normalVec), body1->InverseInertiaTensor()),
			rb));


	float term3 = DirectX::XMVectorGetX(ta);
	float term4 = DirectX::XMVectorGetX(tb);

	denominator = term1 + term2 + term3 + term4;

	float j = 0;
	j = numerator / denominator;

	DirectX::XMVECTOR impulse = DirectX::XMVectorScale(normalVec, j);
	DirectX::XMStoreFloat3(&body0->_linearVelocity, DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&body0->_linearVelocity),
		DirectX::XMVectorScale(impulse, body0->InverseMass())));
	assert(!DirectX::XMVector3IsNaN(DirectX::XMLoadFloat3(&body0->_linearVelocity)));
	DirectX::XMStoreFloat3(&body1->_linearVelocity, DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&body1->_linearVelocity),
		DirectX::XMVectorScale(impulse, body1->InverseMass())));
	assert(!DirectX::XMVector3IsNaN(DirectX::XMLoadFloat3(&body1->_linearVelocity)));

	DirectX::XMStoreFloat3(&body0->_angularVelocity, DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&body0->_angularVelocity),
		DirectX::XMVector3TransformCoord(DirectX::XMVector3Cross(ra, impulse), body0->InverseInertiaTensor())));
	assert(!DirectX::XMVector3IsNaN(DirectX::XMLoadFloat3(&body0->_angularVelocity)));
	DirectX::XMStoreFloat3(&body1->_angularVelocity, DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&body1->_angularVelocity),
		DirectX::XMVector3TransformCoord(DirectX::XMVector3Cross(rb, impulse), body1->InverseInertiaTensor())));
	assert(!DirectX::XMVector3IsNaN(DirectX::XMLoadFloat3(&body1->_angularVelocity)));

	//めり込み量の解決
	{
		auto deno = body0->_inertialMass + body1->_inertialMass;
		Vector3 position = body0->GetActor()->GetTransform().GetPosition();
		DirectX::XMStoreFloat3(&position, DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&position),
			DirectX::XMVectorScale(normalVec, collisionData.penetration * (body1->_inertialMass / deno))));
		assert(!DirectX::XMVector3IsNaN(DirectX::XMLoadFloat3(&position)));
		body0->GetActor()->GetTransform().SetPosition(position);

		position = body1->GetActor()->GetTransform().GetPosition();
		DirectX::XMStoreFloat3(&position, DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&position),
			DirectX::XMVectorScale(normalVec, collisionData.penetration * (body0->_inertialMass / deno))));
		assert(!DirectX::XMVector3IsNaN(DirectX::XMLoadFloat3(&position)));
		body1->GetActor()->GetTransform().SetPosition(position);
	}

	// 摩擦の計算
	// https://qiita.com/k5o/items/119d8fbdbd56e01c81db
	// https://qiita.com/comefrombottom/items/244cdf905e9275b5ee5f
	if (true/*frictionFlg*/)
	{
		// 速度
		const DirectX::XMVECTOR b0Velocity = DirectX::XMLoadFloat3(&body0->_linearVelocity);
		// 衝突前の速度
		const DirectX::XMVECTOR b0OldVelocity = DirectX::XMLoadFloat3(&body0->_oldLinearVelocity);
		// 速度
		const DirectX::XMVECTOR b1Velocity = DirectX::XMLoadFloat3(&body1->_linearVelocity);
		// 衝突前の速度
		const DirectX::XMVECTOR b1OldVelocity = DirectX::XMLoadFloat3(&body1->_oldLinearVelocity);

		// body0の処理
		{
			// 接触点に対しての垂直抗力
			const float normalForce = -DirectX::XMVectorGetX(
				DirectX::XMVector3Dot(b0OldVelocity, normalVec)
			);
			if (normalForce > 0.0f)
			{
				// 摩擦力
				const float frictionForce = body0->_isStandstill == true ?
					body1->_staticFriction * normalForce :
					body1->_dynamicFriction * normalForce;

				// 接触点に対しての水平方向の速度を取得
					// 接触面と速度の内積
				const float dotVN = DirectX::XMVectorGetX(
					DirectX::XMVector3Dot(normalVec, b0OldVelocity)
				);

				// 接触面の法線を平面とみなし、速度をその平面に射影
				DirectX::XMVECTOR projection =
					DirectX::XMVectorSubtract(b0OldVelocity,
						DirectX::XMVectorScale(
							normalVec, dotVN
						)
					);

				// 摩擦力が水平方向への速度量より大きければ速度を接触相手のものにする
				if (DirectX::XMVectorGetX(DirectX::XMVector3Length(projection)) < frictionForce)
				{
					DirectX::XMStoreFloat3(&body0->_linearVelocity, b1Velocity);
				}
				else
				{
					body0->AddForceAtPoint(DirectX::XMVectorScale(
						DirectX::XMVector3Normalize(projection),
						-frictionForce),
						pointVec);
				}
			}

		}
		// body1の処理
		{
			// 接触点に対しての垂直抗力
			const float normalForce = -DirectX::XMVectorGetX(
				DirectX::XMVector3Dot(b1OldVelocity, normalVec)
			);

			if (normalForce > 0.0f)
			{
				// 摩擦力
				const float frictionForce = body1->_isStandstill == true ?
					body0->_staticFriction * normalForce :
					body0->_dynamicFriction * normalForce;

				// 接触点に対しての水平方向の速度を取得
					// 接触面と速度の内積
				const float dotVN = DirectX::XMVectorGetX(
					DirectX::XMVector3Dot(normalVec, b1OldVelocity)
				);

				// 接触面の法線を平面とみなし、速度をその平面に射影
				DirectX::XMVECTOR projection =
					DirectX::XMVectorSubtract(b1OldVelocity,
						DirectX::XMVectorScale(
							normalVec, dotVN
						)
					);

				// 摩擦力が水平方向への速度量より大きければ速度を接触相手のものにする
				if (DirectX::XMVectorGetX(DirectX::XMVector3Length(projection)) < frictionForce)
				{
					DirectX::XMStoreFloat3(&body1->_linearVelocity, b0Velocity);
				}
				else
				{
					body1->AddForceAtPoint(DirectX::XMVectorScale(
						DirectX::XMVector3Normalize(projection),
						-frictionForce),
						pointVec);
				}
			}
		}
	}
}
