#pragma once

#include "Vector.h"
#include "Quaternion.h"

class Matrix4X4 : public DirectX::XMFLOAT4X4
{
public:
	Matrix4X4() : DirectX::XMFLOAT4X4()
	{
		DirectX::XMStoreFloat4x4(this, DirectX::XMMatrixIdentity());
	};
	Matrix4X4(const DirectX::XMFLOAT4X4& m) : DirectX::XMFLOAT4X4(m) {};
	Matrix4X4(const DirectX::XMMATRIX& m)
	{
		DirectX::XMStoreFloat4x4(this, m);
	}
	~Matrix4X4() {};

#pragma region 演算子オーバーロード
	Matrix4X4 operator=(const Matrix4X4&);
	Matrix4X4 operator*(const Matrix4X4&) const;
#pragma endregion

#pragma region 静的メンバ変数
	static constexpr DirectX::XMFLOAT4X4 Identity =
	{
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};
#pragma endregion

#pragma region 静的メンバ関数
	// 平行移動行列取得
	static Matrix4X4 Translation(float x, float y, float z)
	{
		Matrix4X4 m;
		DirectX::XMStoreFloat4x4(&m, DirectX::XMMatrixTranslation(x, y, z));
		return m;
	}
	// 拡大縮小行列取得
	static Matrix4X4 Scaling(float sx, float sy, float sz)
	{
		Matrix4X4 m;
		DirectX::XMStoreFloat4x4(&m, DirectX::XMMatrixScaling(sx, sy, sz));
		return m;
	}
	// 回転行列取得(X軸回転)
	static Matrix4X4 RotationX(float radian)
	{
		Matrix4X4 m;
		DirectX::XMStoreFloat4x4(&m, DirectX::XMMatrixRotationX(radian));
		return m;
	}
	// 回転行列取得(Y軸回転)
	static Matrix4X4 RotationY(float radian)
	{
		Matrix4X4 m;
		DirectX::XMStoreFloat4x4(&m, DirectX::XMMatrixRotationY(radian));
		return m;
	}
	// 回転行列取得(Z軸回転)
	static Matrix4X4 RotationZ(float radian)
	{
		Matrix4X4 m;
		DirectX::XMStoreFloat4x4(&m, DirectX::XMMatrixRotationZ(radian));
		return m;
	}
	// 回転行列取得(オイラー回転 ZXY)
	static Matrix4X4 RotationRollPitchYaw(float pitch, float yaw, float roll)
	{
		Matrix4X4 m;
		DirectX::XMStoreFloat4x4(&m, DirectX::XMMatrixRotationRollPitchYaw(pitch, yaw, roll));
		return m;
	}
	// 回転行列取得(クォータニオン)
	static Matrix4X4 RotationQuaternion(const DirectX::XMFLOAT4& q)
	{
		Matrix4X4 m;
		DirectX::XMStoreFloat4x4(&m, DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&q)));
		return m;
	}
	// 逆行列取得
	static Matrix4X4 Inverse(const Matrix4X4& m)
	{
		Matrix4X4 result;
		DirectX::XMStoreFloat4x4(&result, DirectX::XMMatrixInverse(nullptr, DirectX::XMLoadFloat4x4(&m)));
		return result;
	}
	// 行列分解
	static void Decompose(const Matrix4X4& m, Vector3& scale, Quaternion& rotation, Vector3& translation)
	{
		DirectX::XMVECTOR s, r, t;
		DirectX::XMMatrixDecompose(&s, &r, &t, DirectX::XMLoadFloat4x4(&m));
		DirectX::XMFLOAT3 fs, ft;
		DirectX::XMFLOAT4 fr;
		DirectX::XMStoreFloat3(&fs, s);
		DirectX::XMStoreFloat4(&fr, r);
		DirectX::XMStoreFloat3(&ft, t);
		scale = Vector3(fs);
		rotation = fr;
		translation = Vector3(ft);
	}
	// 行列分解
	static void Decompose(const Matrix4X4& m, Vector3& scale, Vector3& rotation, Vector3& translation)
	{
		DirectX::XMVECTOR s, r, t;
		DirectX::XMMatrixDecompose(&s, &r, &t, DirectX::XMLoadFloat4x4(&m));
		DirectX::XMFLOAT3 fs, ft;
		DirectX::XMFLOAT4 fr;
		DirectX::XMStoreFloat3(&fs, s);
		DirectX::XMStoreFloat4(&fr, r);
		DirectX::XMStoreFloat3(&ft, t);
		scale = Vector3(fs);
		rotation = Quaternion::ToRollPitchYaw(fr);
		translation = Vector3(ft);
	}
	// 変換行列作成
	static Matrix4X4 CreateTransform(const Vector3& scale, const Vector3& angle, const Vector3& position)
	{
		DirectX::XMMATRIX T = DirectX::XMMatrixTranslationFromVector(DirectX::XMLoadFloat3(&position));
		DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
		DirectX::XMMATRIX S = DirectX::XMMatrixScalingFromVector(DirectX::XMLoadFloat3(&scale));
		Matrix4X4 m{};
		DirectX::XMStoreFloat4x4(&m, S * R * T);
		return m;
	}
#pragma endregion

#pragma region 関数
	// 逆行列取得
	Matrix4X4 Inverse() const
	{
		return Matrix4X4::Inverse(*this);
	}
	// 行列分解
	void Decompose(Vector3& scale, Quaternion& rotation, Vector3& translation) const
	{
		Matrix4X4::Decompose(*this, scale, rotation, translation);
	}
	// 行列分解
	void Decompose(Vector3& scale, Vector3& rotation, Vector3& translation) const
	{
		Matrix4X4::Decompose(*this, scale, rotation, translation);
	}
	DirectX::XMMATRIX ToXMMATRIX() const
	{
		return DirectX::XMLoadFloat4x4(this);
	}
#pragma endregion
};
