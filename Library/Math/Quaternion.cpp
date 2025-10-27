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
Quaternion Quaternion::LookAt(const DirectX::XMFLOAT3& position,
	const DirectX::XMFLOAT3& target,
	const DirectX::XMFLOAT3& up)
{
	DirectX::XMVECTOR posVec = DirectX::XMLoadFloat3(&position);
	DirectX::XMVECTOR targetVec = DirectX::XMLoadFloat3(&target);
	DirectX::XMVECTOR upVec = DirectX::XMLoadFloat3(&up);

	// 新しいZ軸（前方）を計算 = (target - position) の正規化
	DirectX::XMVECTOR zaxis = DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(targetVec, posVec));

	// 稀にpositionとtargetが全く同じ座標の場合
	if (DirectX::XMVector3Equal(DirectX::XMVector3LengthSq(zaxis), DirectX::g_XMZero))
	{
		Quaternion q{};
		DirectX::XMStoreFloat4(&q, DirectX::XMQuaternionIdentity());
		return q;
	}

	// 新しいX軸（右）を計算 = (up × zaxis) の正規化
	DirectX::XMVECTOR xaxis = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(upVec, zaxis));
	
	// X軸の計算が失敗したかチェック（ベクトル長がほぼ0か）
	if (DirectX::XMVector3Less(DirectX::XMVector3LengthSq(xaxis), DirectX::g_XMEpsilon))
	{
		// 代わりのX軸を (ワールドX軸 × zaxis) から計算
		DirectX::XMVECTOR worldRight = DirectX::XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
		xaxis = DirectX::XMVector3Cross(worldRight, zaxis);

		// もしzaxisがワールドX軸とも平行
		if (DirectX::XMVector3Less(DirectX::XMVector3LengthSq(xaxis), DirectX::g_XMEpsilon))
		{
			// zaxisはX軸に平行だったので、ワールドY軸を使う
			DirectX::XMVECTOR worldUp = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
			xaxis = DirectX::XMVector3Cross(worldUp, zaxis);
		}
	}

	// 新しいY軸（上）を計算 = (zaxis × xaxis)
	DirectX::XMVECTOR yaxis = DirectX::XMVector3Cross(zaxis, xaxis);

	// 5. 3つの軸ベクトルから回転行列を構築
	DirectX::XMMATRIX rotationMatrix;
	rotationMatrix.r[0] = xaxis; // X軸
	rotationMatrix.r[1] = yaxis; // Y軸
	rotationMatrix.r[2] = zaxis; // Z軸
	rotationMatrix.r[3] = DirectX::XMVECTOR{ 0, 0, 0, 1 }; // 4行目 (0, 0, 0, 1)

	// 回転行列からクォータニオンを生成して返す
    Quaternion q{};
    DirectX::XMStoreFloat4(&q,
        DirectX::XMQuaternionNormalize(DirectX::XMQuaternionRotationMatrix(rotationMatrix)));
	return q;
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
