#include "Quaternion.h"

Quaternion QuaternionRotationAxis(const DirectX::XMFLOAT3& axis, float radian)
{
	float sin = sinf(radian / 2.0f);
	Quaternion Q{};
	Q.x = axis.x * sin;
	Q.y = axis.y * sin;
	Q.z = axis.z * sin;
	Q.w = cosf(radian / 2.0f);
	return Q;
}

Quaternion QuaternionRotationAxisDegree(const DirectX::XMFLOAT3& axis, float degree)
{
	return QuaternionRotationAxis(axis, DirectX::XMConvertToRadians(degree));
}

Quaternion QuaternionRotationAxis(const DirectX::XMVECTOR& axis, float radian)
{
	Quaternion Q{};
	DirectX::XMStoreFloat4(&Q,
		DirectX::XMQuaternionRotationAxis(axis, radian));
	return Q;
}

Quaternion QuaternionRotationAxisDegree(const DirectX::XMVECTOR& axis, float degree)
{
	return QuaternionRotationAxis(axis, DirectX::XMConvertToRadians(degree));
}

//--------------------------------------------------
// クォータニオンの掛け算
Quaternion QuaternionMultiply(const Quaternion& src, const Quaternion& dst)
{
	Quaternion Q{};
	DirectX::XMStoreFloat4(&Q,
		DirectX::XMQuaternionMultiply(
			DirectX::XMLoadFloat4(&src),
			DirectX::XMLoadFloat4(&dst)));
	return Q;
}

//--------------------------------------------------
// 指定方向に向くクォータニオン
// position	: ワールド座標
// front	: 「正規化された」前方向
// target	: 向く方向
Quaternion QuaternionLookAt(const DirectX::XMFLOAT3& position,
	const DirectX::XMFLOAT3& front,
	const DirectX::XMFLOAT3& target)
{
	Quaternion Q{};
	// positionからtargetまでのベクトルと前方向のベクトルで回転軸を算出
	DirectX::XMVECTOR Front = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&front));
	DirectX::XMVECTOR PosToTarget = DirectX::XMVectorSubtract(
		DirectX::XMLoadFloat3(&target),
		DirectX::XMLoadFloat3(&position)
	);
	PosToTarget = DirectX::XMVector3Normalize(PosToTarget);
	DirectX::XMVECTOR Axis = DirectX::XMVector3Cross(Front, PosToTarget);
	float angle = 0.0f;
	// 外積の計算が失敗しているなら前方向と指定方向が同じ
	if (DirectX::XMVectorGetX(DirectX::XMVector3LengthSq(Axis)) == 0.0f || DirectX::XMVector3IsNaN(Axis))
	{
		DirectX::XMStoreFloat4(&Q, DirectX::XMQuaternionNormalize(Axis));
	}
	else
	{
		// 回転角を求める
		float angle = acosf(
			DirectX::XMVectorGetX(
				DirectX::XMVector3Dot(Front, PosToTarget)));

		DirectX::XMStoreFloat4(&Q,
			DirectX::XMQuaternionNormalize(DirectX::XMQuaternionRotationAxis(Axis, angle)));
	}

	return Q;
}

Quaternion QuaternionLookAt(const Quaternion& q, 
	const DirectX::XMFLOAT3& position,
	const DirectX::XMFLOAT3& front,
	const DirectX::XMFLOAT3& target)
{
	Quaternion Q{};
	DirectX::XMMATRIX R = DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&q));
	// positionからtargetまでのベクトルと前方向のベクトルで回転軸を算出
	DirectX::XMVECTOR Front = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&front));
	DirectX::XMVECTOR PosToTarget = DirectX::XMVectorSubtract(
		DirectX::XMLoadFloat3(&target),
		DirectX::XMLoadFloat3(&position)
	);
	PosToTarget = DirectX::XMVector3Normalize(PosToTarget);
	DirectX::XMVECTOR Axis = DirectX::XMVector3Cross(Front, PosToTarget);
	if (DirectX::XMVector3Equal(Axis, DirectX::XMVectorZero()))
	{
		return q;
	}

	float angle = acosf(
		DirectX::XMVectorGetX(
			DirectX::XMVector3Dot(Front, PosToTarget)));

	// NANチェック
	if(angle > 0.0f || angle < 0.0f)
		DirectX::XMStoreFloat4(&Q,
			DirectX::XMQuaternionRotationAxis(Axis, angle));

	return QuaternionMultiply(q, Q);
}

DirectX::XMFLOAT3 QuaternionToRollPitchYaw(const Quaternion& q)
{
	DirectX::XMFLOAT3 angle{};
	// クォータニオンから回転行列を算出
	DirectX::XMVECTOR Q = DirectX::XMLoadFloat4(&q);
	DirectX::XMMATRIX M = DirectX::XMMatrixRotationQuaternion(Q);
	DirectX::XMFLOAT4X4 m;
	DirectX::XMStoreFloat4x4(&m, M);

	// 回転行列からヨー、ピッチ、ロールを計算
	float xRadian = asinf(-m._32);
	angle.x = xRadian;
	if (xRadian < DirectX::XM_PI / 2.0f)
	{
		if (xRadian > -DirectX::XM_PI / 2.0f)
		{
			angle.z = atan2f(m._12, m._22);
			angle.y = atan2f(m._31, m._33);
		}
		else
		{
			angle.z = (float)-atan2f(m._13, m._11);
			angle.y = 0.0f;
		}
	}
	else
	{
		angle.z = (float)atan2f(m._13, m._11);
		angle.y = 0.0f;
	}

	return angle;
}

DirectX::XMFLOAT3 QuaternionToRollPitchYaw(const DirectX::XMVECTOR& q)
{
	DirectX::XMFLOAT3 angle{};
	// クォータニオンから回転行列を算出
	DirectX::XMMATRIX M = DirectX::XMMatrixRotationQuaternion(q);
	DirectX::XMFLOAT4X4 m;
	DirectX::XMStoreFloat4x4(&m, M);

	// 回転行列からヨー、ピッチ、ロールを計算
	float xRadian = asinf(-m._32);
	angle.x = xRadian;
	if (xRadian < DirectX::XM_PI / 2.0f)
	{
		if (xRadian > -DirectX::XM_PI / 2.0f)
		{
			angle.z = atan2f(m._12, m._22);
			angle.y = atan2f(m._31, m._33);
		}
		else
		{
			angle.z = (float)-atan2f(m._13, m._11);
			angle.y = 0.0f;
		}
	}
	else
	{
		angle.z = (float)atan2f(m._13, m._11);
		angle.y = 0.0f;
	}

	return angle;
}

Quaternion QuaternionFromRollPitchYaw(const DirectX::XMFLOAT3& angle)
{
	Quaternion q{};
	DirectX::XMStoreFloat4(&q,
		DirectX::XMQuaternionRotationRollPitchYawFromVector(DirectX::XMLoadFloat3(&angle)));
	return q;
}

Quaternion QuaternionSlerp(const Quaternion& src, const Quaternion& dst, float t)
{
	Quaternion q{};
	DirectX::XMStoreFloat4(&q,
		DirectX::XMQuaternionSlerp(DirectX::XMLoadFloat4(&src), DirectX::XMLoadFloat4(&dst), t));
	return q;
}

Quaternion::Quaternion(float roll, float pitch, float yaw)
{
	DirectX::XMFLOAT4 Q{};
	DirectX::XMStoreFloat4(&Q,
		DirectX::XMQuaternionRotationRollPitchYaw(pitch, yaw, roll));
	this->x = Q.x;
	this->y = Q.y;
	this->z = Q.z;
	this->w = Q.w;
}

Quaternion::Quaternion(const DirectX::XMFLOAT4X4& r) :
	Quaternion(DirectX::XMLoadFloat4x4(&r))
{
}

Quaternion::Quaternion(const DirectX::XMMATRIX& r)
{
	DirectX::XMFLOAT4 Q{};
	DirectX::XMStoreFloat4(&Q,
		DirectX::XMQuaternionRotationMatrix(r));
	this->x = Q.x;
	this->y = Q.y;
	this->z = Q.z;
	this->w = Q.w;
}

DirectX::XMFLOAT4X4 Quaternion::ToMatrix() const
{
	DirectX::XMMATRIX M = DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(this));
	DirectX::XMFLOAT4X4 m;
	DirectX::XMStoreFloat4x4(&m, M);
	return m;
}
