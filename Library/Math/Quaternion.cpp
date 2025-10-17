#include "Quaternion.h"

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

#pragma region 静的メンバ関数
// 軸を指定して指定量回転
Quaternion Quaternion::RotationAxis(const DirectX::XMFLOAT3& axis, float radian)
{
	float sin = sinf(radian / 2.0f);
	Quaternion Q{};
	Q.x = axis.x * sin;
	Q.y = axis.y * sin;
	Q.z = axis.z * sin;
	Q.w = cosf(radian / 2.0f);
	return Q;
}
// 軸を指定して指定量回転
Quaternion Quaternion::RotationAxis(const DirectX::XMVECTOR& axis, float radian)
{
    Quaternion Q{};
    DirectX::XMStoreFloat4(&Q,
        DirectX::XMQuaternionRotationAxis(axis, radian));
    return Q;
}
// 軸を指定して指定量回転
Quaternion Quaternion::RotationAxisDegree(const DirectX::XMFLOAT3& axis, float radian)
{
    return Quaternion::RotationAxis(axis, DirectX::XMConvertToRadians(radian));
}
// 軸を指定して指定量回転
Quaternion Quaternion::RotationAxisDegree(const DirectX::XMVECTOR& axis, float radian)
{
    return Quaternion::RotationAxis(axis, DirectX::XMConvertToRadians(radian));
}
// 正規化
Quaternion Quaternion::Normalize(const Quaternion& src)
{
	DirectX::XMVECTOR Q = DirectX::XMQuaternionNormalize(DirectX::XMLoadFloat4(&src));
	Quaternion q{};
	DirectX::XMStoreFloat4(&q, Q);
	return q;
}
// クォータニオンの掛け算
Quaternion Quaternion::Multiply(const Quaternion& src, const Quaternion& dst)
{
	Quaternion Q{};
	DirectX::XMStoreFloat4(&Q,
		DirectX::XMQuaternionMultiply(
			DirectX::XMLoadFloat4(&src),
			DirectX::XMLoadFloat4(&dst)));
	return Q;
}
// 指定方向を向く
Quaternion Quaternion::LookAt(const Quaternion& q, const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& front, const DirectX::XMFLOAT3& target)
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
	if (angle > 0.0f || angle < 0.0f)
		DirectX::XMStoreFloat4(&Q,
			DirectX::XMQuaternionRotationAxis(Axis, angle));

	return Quaternion::Multiply(q, Q);
}
// 指定方向を向く
Quaternion Quaternion::LookAt(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& front, const DirectX::XMFLOAT3& target)
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
	// 外積の計算が失敗しているなら前方向と指定方向が同じ
	if (DirectX::XMVectorGetX(DirectX::XMVector3LengthSq(Axis)) == 0.0f || DirectX::XMVector3IsNaN(Axis))
	{
		DirectX::XMStoreFloat4(&Q, DirectX::XMQuaternionNormalize(Axis));
	}
	else
	{
		Axis = DirectX::XMVector3Normalize(Axis);
		// 回転角を求める
		float angle = acosf(
			DirectX::XMVectorGetX(
				DirectX::XMVector3Dot(Front, PosToTarget)));

		DirectX::XMStoreFloat4(&Q,
			DirectX::XMQuaternionNormalize(DirectX::XMQuaternionRotationAxis(Axis, angle)));
	}

	return Q;
}
// クォータニオンからオイラー角に変換
DirectX::XMFLOAT3 Quaternion::ToRollPitchYaw(const Quaternion& q)
{
    return ToRollPitchYaw(DirectX::XMLoadFloat4(&q));
}
// クォータニオンからオイラー角に変換
DirectX::XMFLOAT3 Quaternion::ToRollPitchYaw(const DirectX::XMVECTOR& q)
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
// オイラー角からクォータニオンに変換
Quaternion Quaternion::FromRollPitchYaw(const DirectX::XMFLOAT3& angle)
{
	Quaternion q{};
	DirectX::XMStoreFloat4(&q,
		DirectX::XMQuaternionRotationRollPitchYawFromVector(DirectX::XMLoadFloat3(&angle)));
	return q;
}
// クォータニオンの保管
Quaternion Quaternion::Slerp(const Quaternion& src, const Quaternion& dst, float t)
{
	Quaternion q{};
	DirectX::XMStoreFloat4(&q,
		DirectX::XMQuaternionSlerp(DirectX::XMLoadFloat4(&src), DirectX::XMLoadFloat4(&dst), t));
	return q;
}
// クォータニオン反転
Quaternion Quaternion::Inverse(const Quaternion& q)
{
	Quaternion Q{};
	DirectX::XMStoreFloat4(&Q,
		DirectX::XMQuaternionInverse(DirectX::XMLoadFloat4(&q)));
	return Q;
}
#pragma endregion

// 回転行列に変換
DirectX::XMFLOAT4X4 Quaternion::ToMatrix() const
{
	DirectX::XMMATRIX M = DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(this));
	DirectX::XMFLOAT4X4 m;
	DirectX::XMStoreFloat4x4(&m, M);
	return m;
}

Quaternion Quaternion::operator*(const Quaternion& q) const
{
	return Quaternion::Multiply(*this, q);
}

Quaternion Quaternion::Multiply(const Quaternion& q) const
{
	return *this * q;
}
