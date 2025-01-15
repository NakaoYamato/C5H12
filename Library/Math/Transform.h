#pragma once
#include "Vector.h"
#include "Quaternion.h"

// 座標系
enum COORDINATE_TYPE
{
	RHS_Y_UP = 0,
	LHS_Y_UP = 1,
	RHS_Z_UP = 2,
	LHS_Z_UP = 3,
};

class Transform
{
public:
	Transform() {}
	~Transform() {}

	// 行列の更新
	void UpdateTransform(const DirectX::XMFLOAT4X4* parent);

	// デバッグGUI表示
	void DrawGui();

	// X軸取得
	Vector3 GetAxisX()const { return Vector3(transform._11, transform._12, transform._13); }
	// Y軸取得
	Vector3 GetAxisY()const { return Vector3(transform._21, transform._22, transform._23); }
	// Z軸取得
	Vector3 GetAxisZ()const { return Vector3(transform._31, transform._32, transform._33); }
	// ワールド座標取得
	Vector3 GetWorldPosition()const { return Vector3(transform._41, transform._42, transform._43); }

	// アクセサ
	const Vector3& GetPosition()const { return position_; }
	const Vector3& GetScale()const { return scale_; }
	const Vector3& GetRotation()const { return angle_; }
	const DirectX::XMFLOAT4X4& GetMatrix()const { return transform; }
	int GetCoordinateType()const { return coordinateType_; }
	float GetLengthScale()const { return lengthScale_; }

	void SetPosition(const Vector3& v) { this->position_ = v; }
	void SetPositionX(float f) { this->position_.x = f; }
	void SetPositionY(float f) { this->position_.y = f; }
	void SetPositionZ(float f) { this->position_.z = f; }
	void SetScale(const Vector3& v) { this->scale_ = v; }
	void SetAngle(const Vector3& v) { this->angle_ = v; }
	void SetAngleX(float f) { this->angle_.x = f; }
	void SetAngleY(float f) { this->angle_.y = f; }
	void SetAngleZ(float f) { this->angle_.z = f; }

	void SetCoordinateType(int type) { this->coordinateType_ = type; }
	void SetLengthScale(float lengthScale) { this->lengthScale_ = lengthScale; }
	void SetMatrix(const DirectX::XMFLOAT4X4& m) { this->transform = m; }

	void AddPosition(const Vector3& v) { this->position_ += v; }
	void AddScale(const Vector3& v) { this->scale_ += v; }
	void AddRotation(const Vector3& v) { this->angle_ += v; }

private:
	int coordinateType_ = COORDINATE_TYPE::LHS_Y_UP;

	// 長さの単位(m)
	float lengthScale_ = 1.0f;

	Vector3 position_{};
	Vector3 scale_{ 1.0f, 1.0f, 1.0f};
	Vector3 angle_{};

	DirectX::XMFLOAT4X4 transform =
	{
		1.0f,0.0f,0.0f,0.0f,
		0.0f,1.0f,0.0f,0.0f,
		0.0f,0.0f,1.0f,0.0f,
		0.0f,0.0f,0.0f,1.0f
	};
};