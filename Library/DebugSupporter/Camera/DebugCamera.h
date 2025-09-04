#pragma once

#include "../../Library/Camera/CameraController.h"

/// <summary>
/// デバッグ用カメラコントローラー
/// </summary>
class DebugCamera : public CameraControllerBase
{
public:
	// 更新処理
	void Update(float elapsedTime)override;

	// デバッグ用Gui
	void DrawGui()override;

	bool _isActive = false;
private:
	float _movePower = 0.01f;
	float _targetMovePower = 0.8f;
	float _zoomPower = 0.5f;
};