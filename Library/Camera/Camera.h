#pragma once

#include "../Math/Vector.h"

/// <summary>
/// カメラ
/// </summary>
class Camera
{
public:
	Camera() {};
	Camera(const Camera&) = delete;
	~Camera() {};

public:
	// 指定方向を向く
	void SetLookAt(const Vector3& eye, const Vector3& focus, const Vector3& up);

	// パースペクティブ設定
	void SetPerspectiveFov(float fovY, float aspect, float nearZ, float farZ);

	// ビュー行列取得
	const DirectX::XMFLOAT4X4& GetView() const { return view; }

	// プロジェクション行列取得
	const DirectX::XMFLOAT4X4& GetProjection() const { return projection; }

	// 視点取得
	const Vector3& GetEye() const { return eye; }

	// 注視点取得
	const Vector3& GetFocus() const { return focus; }

	// 上方向取得
	const Vector3& GetUp() const { return up; }

	// 前方向取得
	const Vector3& GetFront() const { return front; }

	// 右方向取得
	const Vector3& GetRight() const { return right; }
private:
	DirectX::XMFLOAT4X4		view{};
	DirectX::XMFLOAT4X4		projection{};

	Vector3		eye{};
	Vector3		focus{};

	Vector3		up{};
	Vector3		front{};
	Vector3		right{};

	float fovY = 0;
	float aspect = 0;
	float nearZ = 0;
	float farZ = 0;
};
