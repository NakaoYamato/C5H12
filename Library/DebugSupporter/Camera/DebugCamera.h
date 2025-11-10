#pragma once

#include "../../Library/Math/Vector.h"

/// <summary>
/// デバッグ用カメラコントローラー
/// </summary>
class DebugCamera
{
public:
	DebugCamera() {};
	~DebugCamera() {};

	// 更新処理
	void Update(float elapsedTime);

	// デバッグ用Gui
	void DrawGui();

#pragma region アクセサ
	const Vector3& GetTarget()const { return _target; }
	const Vector3& GetEye()const { return _eye; }
	const Vector3& GetAngle()const { return _angle; }
	float GetRange()const { return _range; }

	void SetTarget(const Vector3& target) { this->_target = target; }
	void SetRange(const float& scaleRange) { this->_range = scaleRange; }
	void SetAngle(const Vector3 angle) { this->_angle = angle; }
#pragma endregion


	bool _isActive = false;
private:
	Vector3 _target = { 0,0,0 };
	Vector3 _eye = { 0,0,0 };
	Vector3 _angle = { 0,0,0 };
	float _range = 10.0f;

	float _movePower = 0.01f;
	float _targetMovePower = 0.8f;
	float _zoomPower = 0.5f;
};