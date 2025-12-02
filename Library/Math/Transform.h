#pragma once

#include "Vector.h"
#include "Quaternion.h"
#include "Matrix.h"

class Transform
{
public:
	Transform() {}
	~Transform() {}

	/// <summary>
	/// 行列の更新
	/// </summary>
	/// <param name="parent"></param>
	void UpdateTransform(const DirectX::XMFLOAT4X4* parent);

	/// <summary>
	/// デバッグGUI表示
	/// </summary>
	void DrawGui();

	/// <summary>
	/// 指定方向（ワールド空間）を向く
	/// </summary>
	/// <param name="worldDirection"></param>
	void LookAt(const Vector3& worldDirection);

	/// <summary>
	/// 値をリセット
	/// </summary>
	void Reset();
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
	const Vector3& GetAngle()const { return _angle; }
	/// <summary>
	/// 行列取得(ワールド)
	/// </summary>
	/// <returns></returns>
	const Matrix4X4& GetMatrix()const { return _transform; }
    /// <summary>
    /// 逆行列取得(ワールド)
    /// </summary>
    /// <returns></returns>
	Matrix4X4 GetMatrixInverse()const
    {
        return _transform.Inverse();
    }
	/// <summary>
	/// 単位取得
	/// </summary>
	/// <returns></returns>
	float GetLengthScale()const { return _lengthScale; }

	void SetPosition(const Vector3& v) { this->_position = v; }
	void SetPosition(float v) { this->_position.x = v; this->_position.y = v; this->_position.z = v; }
	void SetPosition(float x, float y, float z) { this->_position.x = x; this->_position.y = y; this->_position.z = z; }
	void SetPositionX(float f) { this->_position.x = f; }
	void SetPositionY(float f) { this->_position.y = f; }
	void SetPositionZ(float f) { this->_position.z = f; }
	void SetScale(const Vector3& v) { this->_scale = v; }
	void SetScale(float v) { this->_scale.x = v; this->_scale.y = v; this->_scale.z = v; }
	void SetScale(float x, float y, float z) { this->_scale.x = x; this->_scale.y = y; this->_scale.z = z; }
	void SetAngle(const Vector3& v) { this->_angle = v; }
	void SetAngle(float v) { this->_angle.x = v; this->_angle.y = v; this->_angle.z = v; }
	void SetAngle(float x, float y, float z) { this->_angle.x = x; this->_angle.y = y; this->_angle.z = z; }
	void SetAngleX(float f) { this->_angle.x = f; }
	void SetAngleY(float f) { this->_angle.y = f; }
	void SetAngleZ(float f) { this->_angle.z = f; }

	void SetLengthScale(float lengthScale) { this->_lengthScale = lengthScale; }
	void SetMatrix(const DirectX::XMFLOAT4X4& m) { this->_transform = m; }

	void AddPosition(const Vector3& v) { this->_position += v; }
	void AddScale(const Vector3& v) { this->_scale += v; }
	void AddAngle(const Vector3& v) { this->_angle += v; }

	// X軸取得
	Vector3 GetAxisX()const { return Vector3(_transform._11, _transform._12, _transform._13); }
	// Y軸取得
	Vector3 GetAxisY()const { return Vector3(_transform._21, _transform._22, _transform._23); }
	// Z軸取得
	Vector3 GetAxisZ()const { return Vector3(_transform._31, _transform._32, _transform._33); }
	// ワールド座標取得
	Vector3 GetWorldPosition()const { return Vector3(_transform._41, _transform._42, _transform._43); }
#pragma endregion

#pragma region ファイル操作
	NLOHMANN_DEFINE_TYPE_INTRUSIVE(Transform, _lengthScale, _position, _scale, _angle)
#pragma endregion
private:
	// 長さの単位(m)
	float _lengthScale = 1.0f;

	Vector3 _position	= Vector3::Zero;
	Vector3 _scale		= Vector3::One;
	Vector3 _angle		= Vector3::Zero;

	Matrix4X4 _transform = Matrix4X4::Identity;
};