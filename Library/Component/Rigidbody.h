#pragma once

#include "Component.h"

class Rigidbody : public Component
{
public:
	Rigidbody() {}
	~Rigidbody() override {}
	// 名前取得
	const char* GetName() const override { return "Rigidbody"; }

	// 開始処理
	void Start() override;
	// 更新処理
	void Update(float elapsedTime) override;
	// 固定間隔更新処理
	void FixedUpdate() override;
	// GUI描画
	void DrawGui() override;
	// 接触の解消処理
	void OnContact(CollisionData& collisionData) override;

	// 力を加える
	// force	: 速度
	void AddForce(const DirectX::XMVECTOR& force);
	void AddForce(const Vector3& force);

	// 回転追加
	void AddTorque(const DirectX::XMVECTOR& torque);
	void AddTorque(const Vector3& torque);

	// 物体のある位置に対して力を加える(慣性モーメント)
	// force	: 力の向きと量
	// point	: 力を加える位置(ワールド座標)
	// 戻り値	: 計算で求めたトルク
	DirectX::XMVECTOR AddForceAtPoint(const DirectX::XMVECTOR& force, const DirectX::XMVECTOR& point/*ワールド座標*/);

#pragma region アクセサ
	const Vector3& GetLinearVelocity() const { return _linearVelocity; }
	const Vector3& GetAngularVelocity() const { return _angularVelocity; }
	float GetDragCoefficient() const { return _dragCoefficient; }
	float GetInertialMass() const { return _inertialMass; }
	float GetStaticFriction() const { return _staticFriction; }
	float GetDynamicFriction() const { return _dynamicFriction; }

	// *注意*　下手に使うと物理挙動が変わる
	void SetLinearVelocity(const Vector3& linearVelocity) { _linearVelocity = linearVelocity; }
	// *注意*　下手に使うと物理挙動が変わる
	void SetAngularVelocity(const Vector3& angularVelocity) { _angularVelocity = angularVelocity; }
	void SetDragCoefficient(float dragCoefficient) { _dragCoefficient = dragCoefficient; }
	void SetInertialMass(float inertialMass) { _inertialMass = inertialMass; }
	void SetStaticFriction(float staticFriction) { _staticFriction = staticFriction; }
	void SetDynamicFriction(float dynamicFriction) { _dynamicFriction = dynamicFriction; }

	void SetIsMovable(bool isMovable) { _isMovable = isMovable; }
	void SetIsRotatable(bool isRotatable) { _isRotatable = isRotatable; }
	void SetIsStandstill(bool isStandstill) { _isStandstill = isStandstill; }

	//可動オブジェクトか？
	bool IsMovable() const { return _isMovable; }
	//回転可能オブジェクトか？
	bool IsRotatable() const { return _isRotatable; }

	//慣性質量(inertialMass)の逆数を返し、不可動オブジェクト場合は0を返す
	float InverseMass() const
	{
		return (_inertialMass > 0 && _inertialMass < FLT_MAX) ? 1.0f / _inertialMass : 0;
	}

	//慣性モーメントテンソル(inertiaTensor)の逆行列を返す
	DirectX::XMMATRIX InverseInertiaTensor(bool transformed = true) const
	{
		DirectX::XMMATRIX inverseInertiaTensor;
		if (IsMovable())
		{
			inverseInertiaTensor = DirectX::XMMatrixInverse(nullptr, DirectX::XMLoadFloat4x4(&_inertiaTensor));
			if (transformed)
			{
				DirectX::XMMATRIX rotation, transposed_rotation;
				rotation = DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&_orientation));
				transposed_rotation = DirectX::XMMatrixTranspose(rotation);
				inverseInertiaTensor = transposed_rotation * inverseInertiaTensor * rotation;
			}
		}
		else
		{
			inverseInertiaTensor = DirectX::XMMatrixIdentity();
			inverseInertiaTensor.r[0].m128_f32[0] = FLT_EPSILON;
			inverseInertiaTensor.r[1].m128_f32[1] = FLT_EPSILON;
			inverseInertiaTensor.r[2].m128_f32[2] = FLT_EPSILON;
		}
		return inverseInertiaTensor;
	}
#pragma endregion
private:
    /// <summary>
    /// 速度更新
    /// </summary>
    /// <param name="deltaTime"></param>
    void UpdateVelocity(float deltaTime);

    /// <summary>
    /// 位置更新
    /// </summary>
    /// <param name="deltaTime"></param>
    void UpdatePosition(float deltaTime);

	/// <summary>
	/// 接触の解消処理
	/// </summary>
	/// <param name="collisionData"></param>
	void ContactResolve(CollisionData& collisionData);

private:
	Quaternion			_orientation = {};			//姿勢(クォータニオン)
	Vector3				_linearVelocity = {};		//並進速度
	Vector3				_angularVelocity = {};		//角速度

	Vector3				_oldPosition = {};			//前のフレームの位置
	Vector3				_oldLinearVelocity = {};	//前のフレームの並進速度
	Vector3				_oldAngularVelocity = {};	//前のフレームの角速度

	float				_inertialMass = 1.0f;		//慣性質量
	DirectX::XMFLOAT4X4 _inertiaTensor = {};		//慣性モーメント

	Vector3				_accumulatedForce = {};		//力のアキュムレータ
	Vector3				_accumulatedTorque = {};	//トルクアキュムレータ

	// 抗力係数
	float _dragCoefficient = 0.5f;
	float _restitution		= 0.5f;//反発係数	

	// 静止摩擦係数
	float				_staticFriction = 0.5f;
	// 動摩擦係数
	float				_dynamicFriction = 0.05f;

	// 各種フラグ
	bool				_isMovable = true;			// 移動可能か
	bool				_isRotatable = true;		// 回転可能か
	bool				_isStandstill = false;		// 静止中か
};