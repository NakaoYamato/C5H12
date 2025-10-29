#pragma once

#include "../../Library/Component/Component.h"
#include "../../Library/Camera/Camera.h"

#include "../../Source/Player/PlayerActor.h"

/// <summary>
/// プレイヤーカメラコントローラー
/// Transform.Position == eye
/// Transform.Rotation == camera angle
/// </summary>
class PlayerCameraController : public Component
{
public:
    PlayerCameraController(PlayerActor* playerActor) : _playerActor(playerActor) {}
    ~PlayerCameraController() override {}
    // 名前取得
    const char* GetName() const override { return "PlayerCameraController"; }
    // 開始処理
    void Start() override;
    // 更新処理
    void Update(float elapsedTime) override;
	// 固定間隔更新処理
    void FixedUpdate() override;
    // GUI描画
    void DrawGui() override;

private:
    PlayerActor* _playerActor;

    Vector3 _currentFocus{};
    Vector3 _currentEye{};

	// X軸の回転制限
	float _angleXLimitHigh = DirectX::XMConvertToRadians(80.0f);
    float _angleXLimitLow = -DirectX::XMConvertToRadians(80.0f);

	float _lookingUpStartAngle = DirectX::XMConvertToRadians(15.0f);
	float _lookingUpAngleValue = DirectX::XMConvertToRadians(35.0f);

    float _focusVerticalOffset = 1.4f;
	float _focusHorizontalOffset = 0.47f;

	float _focusLerpSpeed = 5.0f;
	float _eyeLerpSpeed = 10.0f;

    float _cameraDistance = 5.5f;
    float _horizontalMovePower = 6.0f;
    float _verticalMovePower = 3.0f;
    float _cameraRadius = 0.1f;
};