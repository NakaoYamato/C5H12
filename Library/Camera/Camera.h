#pragma once

#include "../../Library/Math/Vector.h"
#include "../../Library/Math/Matrix.h"

// カメラ
class Camera
{
public:
	Camera() {};
	~Camera() {};

public:
	// 指定方向を向く
	void SetLookAt(const Vector3& eye, const Vector3& focus, const Vector3& up);
	// パースペクティブ設定
	void SetPerspectiveFov(float fovY, float aspect, float nearZ, float farZ);

	// ビュー行列取得
	const Matrix4X4& GetView() const { return _view; }
	// プロジェクション行列取得
	const Matrix4X4& GetProjection() const { return _projection; }
	// 視点取得
	const Vector3& GetEye() const { return _eye; }
	// 注視点取得
	const Vector3& GetFocus() const { return _focus; }
	// 上方向取得
	const Vector3& GetUp() const { return _up; }
	// 前方向取得
	const Vector3& GetFront() const { return _front; }
	// 右方向取得
	const Vector3& GetRight() const { return _right; }

private:
	Matrix4X4		_view{};
	Matrix4X4		_projection{};

	Vector3		_eye{};
	Vector3		_focus{};

	Vector3		_up{};
	Vector3		_front{};
	Vector3		_right{};

	float _fovY = 0;
	float _aspect = 0;
	float _nearZ = 0;
	float _farZ = 0;
};
