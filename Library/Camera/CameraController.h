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
	const Vector3& GetTarget()const { return _target; }
	const Vector3& GetEye()const { return _eye; }
	const Vector3& GetAngle()const { return _angle; }
	float GetRange()const { return _range; }

	void SetTarget(const Vector3& target) { this->_target = target; }
	void SetRange(const float& scaleRange) { this->_range = scaleRange; }
	void SetAngle(const Vector3 angle) { this->_angle = angle; }
protected:
	Vector3 _target = { 0,0,0 };
	Vector3 _eye = { 0,0,0 };
	Vector3 _angle = { 0,0,0 };
	float _range = 10.0f;
};
