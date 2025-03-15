#pragma once

#include "CameraData.h"

/// <summary>
/// カメラ
/// 現在のカメラ情報をどこからでも取得できるようシングルトンパターン
/// </summary>
class Camera
{
private:
	Camera() {};
	Camera(const Camera&) = delete;
	~Camera() {};

public:
	// インスタンス取得
	static Camera& Instance() {
		static Camera ins{};
		return ins;
	};

	// 指定方向を向く
	void SetLookAt(const Vector3& eye, const Vector3& focus, const Vector3& up);

	// パースペクティブ設定
	void SetPerspectiveFov(float fovY, float aspect, float nearZ, float farZ);

	// ビュー行列取得
	const DirectX::XMFLOAT4X4& GetView() const { return _data.view; }

	// プロジェクション行列取得
	const DirectX::XMFLOAT4X4& GetProjection() const { return _data.projection; }

	// 視点取得
	const Vector3& GetEye() const { return _data.eye; }

	// 注視点取得
	const Vector3& GetFocus() const { return _data.focus; }

	// 上方向取得
	const Vector3& GetUp() const { return _data.up; }

	// 前方向取得
	const Vector3& GetFront() const { return _data.front; }

	// 右方向取得
	const Vector3& GetRight() const { return _data.right; }

	// データ取得
	const CameraData& GetDate() const { return _data; }
private:
	CameraData _data;
};
