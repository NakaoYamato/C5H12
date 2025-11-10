#pragma once

#include "../../Library/Component/CameraControllerBase.h"
#include "../../Library/Component/StateController.h"
#include "../../Library/Camera/Camera.h"
#include "../../Library/Component/CameraEventReceiver.h"

#include "../../Source/Player/PlayerActor.h"
#include "../../Source/InGame/InputManager.h"

/// <summary>
/// プレイヤーカメラコントローラー
/// Transform.Position == eye
/// Transform.Rotation == camera angle
/// </summary>
class PlayerCameraController : public CameraControllerBase
{
public:
    PlayerCameraController(PlayerActor* playerActor) : _playerActor(playerActor) {}
    ~PlayerCameraController() override {}
    // 名前取得
    const char* GetName() const override { return "PlayerCameraController"; }
    // 開始処理
    void Start() override;
    // GUI描画
    void DrawGui() override;

    // 更新時処理
    void OnUpdate(float elapsedTime) override;

private:
    PlayerActor* _playerActor;
	std::weak_ptr<InputManager> _inputManager;

	std::weak_ptr<StateController> _stateController;
	std::weak_ptr<CameraEventReceiver> _cameraEventReceiver;

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

#pragma region 抜刀状態のパラメータ
	float _combatFocusVerticalOffset = 1.5f;
	float _combatFocusHorizontalOffset = 0.3f;

	float _combatFocusLerpSpeed = 7.0f;
	float _combatEyeLerpSpeed = 10.0f;
	float _combatCameraDistance = 5.0f;
#pragma endregion

};