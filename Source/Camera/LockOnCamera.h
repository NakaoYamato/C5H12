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
	// デバッグ表示
	void DebugRender(const RenderContext& rc) override;

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
	// ターゲットの画面最小最大位置(NDC座標)
	Vector2 _targetScreenMinPosition{ -0.5f, -0.2f };
	Vector2 _targetScreenMaxPosition{ 0.5f, 0.2f };
    // ターゲットの画面位置許容範囲
    float _targetScreenPositionTolerance = 0.05f;

	// X軸の回転制限
	float _angleXLimitHigh = DirectX::XMConvertToRadians(80.0f);
	float _angleXLimitLow = -DirectX::XMConvertToRadians(80.0f);

	// 回転速度
	float _horizontalMovePower = 1.0f;
	float _verticalMovePower = 0.75f;

	// ターゲットが裏側時の回転速度
	float _behindTargetHorizontalMovePower = 360.0f;
	// ターゲットが画面外の時の回転速度
	float _outOfScreenHorizontalMovePower = 60.0f;
	float _outOfScreenVerticalMovePower = 30.0f;

	// プレイヤーの足元からのオフセット
	float _playerOffset = 1.4f;
	float _focusHorizontalOffset = 0.47f;

	float _cameraDistance = 5.5f;
	float _cameraRadius = 0.1f;

	float _focusLerpSpeed = 5.0f;
	float _eyeLerpSpeed = 10.0f;

	// ターゲット目標位置デバイス表示
    bool _isDrawTargetDebug = false;
    float _debugTargetSize = 5.0f;
};