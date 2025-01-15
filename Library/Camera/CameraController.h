#pragma once

#include "../Math/Vector.h"

/// <summary>
/// カメラコントローラー基底クラス
/// </summary>
class CameraControllerBase
{
public:
	CameraControllerBase() {};
	virtual ~CameraControllerBase() {};

	// 更新処理
	virtual void Update(float elapsedTime) = 0;

	// デバッグ用Gui
	virtual void DrawGui();

	// アクセサ
	const Vector3& GetTarget()const { return target_; }
	const Vector3& GetEye()const { return eye_; }
	const Vector3& GetAngle()const { return angle_; }
	float GetRange()const { return range_; }

	void SetTarget(const Vector3& target) { this->target_ = target; }
	void SetRange(const float& scaleRange) { this->range_ = scaleRange; }
	void SetAngle(const Vector3 angle) { this->angle_ = angle; }
protected:
	Vector3 target_ = { 0,0,0 };
	Vector3 eye_ = { 0,0,0 };
	Vector3 angle_ = { 0,0,0 };
	float range_ = 10.0f;
};
