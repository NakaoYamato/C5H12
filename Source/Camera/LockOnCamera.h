#pragma once

#include "../../Library/Component/CameraControllerBase.h"
#include "../../Library/Camera/Camera.h"
#include "../../Source/Common/Targetable.h"

#include "../../Source/Player/PlayerActor.h"
#include "../../Source/InGame/InputManager.h"

class LockOnCamera : public CameraControllerBase
{
public:
	LockOnCamera() {}
	~LockOnCamera() override {}
	// 名前取得
	const char* GetName() const override { return "LockOnCamera"; }
	// 開始時処理
	void OnStart() override;
	// 起動時関数
	void OnEntry() override;
	// 更新時処理
	void OnUpdate(float elapsedTime) override;
	// GUI描画
	void DrawGui() override;

	// ターゲット設定
	void SetTarget(Targetable* target);
	// プレイヤーアクター設定
	void SetPlayerActor(PlayerActor* playerActor);

private:
	// ロックオンターゲット
	Targetable* _target = nullptr;
	// プレイヤー
	PlayerActor* _playerActor = nullptr;

	std::weak_ptr<InputManager> _inputManager;

	Vector3 _currentFocus{};
	Vector3 _currentEye{};

	// ターゲットの画面位置(NDC座標)
	Vector2 _targetScreenPosition{};

	// X軸の回転制限
	float _angleXLimitHigh = DirectX::XMConvertToRadians(80.0f);
	float _angleXLimitLow = -DirectX::XMConvertToRadians(80.0f);

	// 回転速度
	float _horizontalMovePower = 3.0f;
	float _verticalMovePower = 1.5f;

	// ターゲットが裏側時の回転速度
	float _behindTargetHorizontalMovePower = 6.0f;
	// ターゲットが画面外の時の回転速度
	float _outOfScreenHorizontalMovePower = 6.0f;
	float _outOfScreenVerticalMovePower = 4.0f;

	// プレイヤーの足元からのオフセット
	float _playerOffset = 1.4f;
	float _focusHorizontalOffset = 0.47f;

	float _cameraDistance = 5.5f;
	float _cameraRadius = 0.1f;

	float _focusLerpSpeed = 5.0f;
	float _eyeLerpSpeed = 10.0f;
};