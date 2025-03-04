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

	bool isActive_ = false;
private:
	float movePower_ = 0.01f;
	float targetMovePower_ = 0.8f;
	float zoomPower_ = 0.5f;
};