#pragma once

#include "Component.h"

class CharactorController : public Component
{
public:
    CharactorController() {}
    ~CharactorController() override {}
    // 名前取得
    const char* GetName() const override { return "CharactorController"; }

    // 更新処理
    void Update(float elapsedTime) override;
    // 固定間隔更新処理
    void FixedUpdate() override;
    // GUI描画
    void DrawGui() override;

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

    Vector3 GetVelocity() const { return _velocity; }
    Vector3 GetAcceleration() const { return _acceleration; }
    float GetMaxSpeedXZ() const { return _maxSpeedXZ; }
    float GetCurrentSpeedXZ() const { return _currentSpeedXZ; }
    Vector3 GetGravity() const { return _gravity; }
    bool IsUseGravity() const { return _useGravity; }

    void SetRadius(float radius) { _radius = radius; }
    void SetSkinWidth(float skinWidth) { _skinWidth = skinWidth; }
    void SetStepOffset(float stepOffset) { _stepOffset = stepOffset; }

    void SetVelocity(const Vector3& velocity) { _velocity = velocity; }
    void SetAcceleration(const Vector3& acceleration) { _acceleration = acceleration; }
    void SetMaxSpeedXZ(float maxSpeedXZ) { _maxSpeedXZ = maxSpeedXZ; }
    void SetCurrentSpeedXZ(float currentSpeedXZ) { _currentSpeedXZ = currentSpeedXZ; }
    void SetGravity(const Vector3& gravity) { _gravity = gravity; }
    void SetUseGravity(bool useGravity) { _useGravity = useGravity; }
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
    /// 移動＆滑り
    /// スフィアキャストで当たり判定
    /// </summary>
    /// <param name="move"></param>
    /// <param name="vertical">trueで垂直処理</param>
    void MoveAndSlide(const Vector3& move, bool vertical);
private:
#pragma region パラメータ
    // 半径
    float _radius = 0.5f;
    // スキン幅
    float _skinWidth = 0.1f;
    // ステップオフセット
    float _stepOffset = 0.1f;

#pragma endregion

#pragma region 移動関係
    Vector3 _velocity = {};
    Vector3 _acceleration = {};

    float _maxSpeedXZ = 10.0f;
    float _currentSpeedXZ = 0.0f;

    Vector3 _gravity = { 0.0f, -9.8f, 0.0f };

    bool _useGravity = false;
#pragma endregion
};