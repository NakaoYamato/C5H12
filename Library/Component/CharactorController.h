#pragma once

#include "Component.h"
#include "Animator.h"

class CharactorController : public Component
{
public:
    CharactorController() {}
    ~CharactorController() override {}
    // 名前取得
    const char* GetName() const override { return "CharactorController"; }

	// 初期化処理
    void Start() override;
    // 遅延更新処理
    void LateUpdate(float elapsedTime) override;
    // 固定間隔更新処理
    void FixedUpdate() override;
    // デバッグ描画処理
    void DebugRender(const RenderContext& rc) override;
    // GUI描画
    void DrawGui() override;
    // 接触の解消処理
    virtual void OnContact(CollisionData& collisionData) override;

    // 力を加える
    void AddForce(const Vector3& force)
    {
        _acceleration += force;
    }

    // 速度を0にする
    void ClearVelocity()
    {
        _velocity = {};
        _acceleration = {};
    }

#pragma region アクセサ
    float GetRadius() const { return _radius; }
    float GetSkinWidth() const { return _skinWidth; }
    float GetStepOffset() const { return _stepOffset; }
    float GetSlopeLimit() const { return _slopeLimit; }
    float GetMass() const { return _mass; }

    const Vector3& GetVelocity() const { return _velocity; }
	Vector3 GetVelocityXZ() const { return { _velocity.x, 0.0f, _velocity.z }; }
    const Vector3& GetAcceleration() const { return _acceleration; }
    float GetMaxSpeedXZ() const { return _maxSpeedXZ; }
    float GetCurrentSpeedXZ() const { return _currentSpeedXZ; }
    const Vector3& GetGravity() const { return _gravity; }
    bool IsUseGravity() const { return _useGravity; }
	bool IsRotateToDirection() const { return _rotateToDirection; }
	bool IsRotateToDirectionByRootMotion() const { return _rotateToDirectionByRootMotion; }

    void SetRadius(float radius) { _radius = radius; }
    void SetSkinWidth(float skinWidth) { _skinWidth = skinWidth; }
    void SetStepOffset(float stepOffset) { _stepOffset = stepOffset; }
    void SetSlopeLimit(float slopeLimit) { _slopeLimit = slopeLimit; }
    void SetMass(float mass) { _mass = mass; }

    void SetVelocity(const Vector3& velocity) { _velocity = velocity; }
    void SetAcceleration(const Vector3& acceleration) { _acceleration = acceleration; }
    void SetMaxSpeedXZ(float maxSpeedXZ) { _maxSpeedXZ = maxSpeedXZ; }
    void SetCurrentSpeedXZ(float currentSpeedXZ) { _currentSpeedXZ = currentSpeedXZ; }
    void SetGravity(const Vector3& gravity) { _gravity = gravity; }
    void SetUseGravity(bool useGravity) { _useGravity = useGravity; }
	void SetRotateToDirection(bool rotateToDirection) { _rotateToDirection = rotateToDirection; }
	void SetRotateToDirectionByRootMotion(bool rotateToDirection) { _rotateToDirectionByRootMotion = rotateToDirection; }
#pragma endregion
public:
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
    /// 移動＆滑り
    /// スフィアキャストで当たり判定
    /// </summary>
    /// <param name="move"></param>
    /// <param name="vertical">trueで垂直処理</param>
    void MoveAndSlide(const Vector3& move, bool vertical);

	/// <summary>
	/// 回転処理
	/// </summary>
	/// <param name="deltaTime"></param>
	void UpdateRotation(float deltaTime, const Vector2& vec);

private:
#pragma region パラメータ
    // 半径
    float _radius = 0.5f;
    // スキン幅
    float _skinWidth = 0.01f;
    // ステップオフセット
    float _stepOffset = 0.1f;
    // スロープ
	float _slopeLimit = DirectX::XMConvertToRadians(45.0f);
    // 質量
    float _mass = 1.0f;
#pragma endregion

#pragma region 移動関係
    Vector3 _velocity = {};
    Vector3 _acceleration = {};
    // 重力
    Vector3 _gravity = { 0.0f, -9.8f, 0.0f };

    // XZ方向の最小速度
    float _minSpeedXZ = 0.01f;
	// XZ方向の最大速度
    float _maxSpeedXZ = 10.0f;
	// XZ方向の現在の速度
    float _currentSpeedXZ = 0.0f;
    // 移動方向への回転速度
	float _rotationSpeed = DirectX::XMConvertToRadians(720.0f);

	// 重力使用
    bool _useGravity = true;
    // 移動方向に向くか
    bool _rotateToDirection = true;
    // ルートモーションによる移動方向に向くか
    bool _rotateToDirectionByRootMotion = false;
#pragma endregion

#pragma region アニメーション
	// アニメーター
    std::weak_ptr<Animator> _animator;
#pragma endregion

#pragma region 衝突関係
    // 押し出し量
    Vector3 _pushOut = {};
#pragma endregion
};