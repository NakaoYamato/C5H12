#pragma once

#include "Vector.h"
#include "Quaternion.h"

class Transform
{
public:
	Transform() {}
	~Transform() {}

	// 行列の更新
	void UpdateTransform(const DirectX::XMFLOAT4X4* parent);

	// デバッグGUI表示
	void DrawGui();

#pragma region アクセサ
	/// <summary>
	/// 座標取得(ローカル)
	/// </summary>
	/// <returns></returns>
	const Vector3& GetPosition()const { return _position; }
	/// <summary>
	/// サイズ取得(ローカル)
	/// </summary>
	/// <returns></returns>
	const Vector3& GetScale()const { return _scale; }
	/// <summary>
	/// 回転（オイラー）取得(ローカル)
	/// </summary>
	/// <returns></returns>
	const Vector3& GetRotation()const { return _angle; }
	/// <summary>
	/// 行列取得(ワールド)
	/// </summary>
	/// <returns></returns>
	const DirectX::XMFLOAT4X4& GetMatrix()const { return _transform; }
	/// <summary>
	/// 単位取得
	/// </summary>
	/// <returns></returns>
	float GetLengthScale()const { return _lengthScale; }

	void SetPosition(const Vector3& v) { this->_position = v; }
	void SetPosition(const float& v) { this->_position.x = v; this->_position.y = v; this->_position.z = v; }
	void SetPositionX(float f) { this->_position.x = f; }
	void SetPositionY(float f) { this->_position.y = f; }
	void SetPositionZ(float f) { this->_position.z = f; }
	void SetScale(const Vector3& v) { this->_scale = v; }
	void SetScale(const float& v) { this->_scale.x = v; this->_scale.y = v; this->_scale.z = v; }
	void SetRotation(const Vector3& v) { this->_angle = v; }
	void SetRotation(const float& v) { this->_angle.x = v; this->_angle.y = v; this->_angle.z = v; }
	void SetAngleX(float f) { this->_angle.x = f; }
	void SetAngleY(float f) { this->_angle.y = f; }
	void SetAngleZ(float f) { this->_angle.z = f; }

	void SetLengthScale(float lengthScale) { this->_lengthScale = lengthScale; }
	void SetMatrix(const DirectX::XMFLOAT4X4& m) { this->_transform = m; }

	void AddPosition(const Vector3& v) { this->_position += v; }
	void AddScale(const Vector3& v) { this->_scale += v; }
	void AddRotation(const Vector3& v) { this->_angle += v; }

	// X軸取得
	Vector3 GetAxisX()const { return Vector3(_transform._11, _transform._12, _transform._13); }
	// Y軸取得
	Vector3 GetAxisY()const { return Vector3(_transform._21, _transform._22, _transform._23); }
	// Z軸取得
	Vector3 GetAxisZ()const { return Vector3(_transform._31, _transform._32, _transform._33); }
	// ワールド座標取得
	Vector3 GetWorldPosition()const { return Vector3(_transform._41, _transform._42, _transform._43); }
#pragma endregion
private:
	// 長さの単位(m)
	float _lengthScale = 1.0f;

	Vector3 _position{};
	Vector3 _scale{ 1.0f, 1.0f, 1.0f};
	Vector3 _angle{};

	DirectX::XMFLOAT4X4 _transform =
	{
		1.0f,0.0f,0.0f,0.0f,
		0.0f,1.0f,0.0f,0.0f,
		0.0f,0.0f,1.0f,0.0f,
		0.0f,0.0f,0.0f,1.0f
	};
};