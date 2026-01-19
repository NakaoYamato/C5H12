#pragma once

#include "../../Library/Component/CameraControllerBase.h"
#include "../../Library/Component/StateController.h"
#include "../../Library/Camera/Camera.h"

#include "../../Source/Player/PlayerActor.h"
#include "../../Source/Player/StateMachine/PlayerStateController.h"
#include "../../Source/InGame/InputManager.h"

/// <summary>
/// プレイヤーカメラコントローラー
/// Transform.Position == eye
/// Transform.Rotation == camera angle
/// </summary>
class PlayerCameraController : public CameraControllerBase
{
public:
    PlayerCameraController() {}
    ~PlayerCameraController() override {}
    // 名前取得
    const char* GetName() const override { return "PlayerCameraController"; }
    // 開始時処理
    void OnStart() override;
    // 起動時関数
    void OnActivate() override;
    // GUI描画
    void DrawGui() override;

    // 更新時処理
    void OnUpdate(float elapsedTime) override;

	// プレイヤーアクター設定
    void SetPlayerActor(PlayerActor* playerActor);
    // リセット
	void Reset(const Vector3& angle);

private:
    PlayerActor* _playerActor = nullptr;
	std::weak_ptr<InputManager> _inputManager;

	std::weak_ptr<PlayerStateController> _stateController;

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