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
	const DirectX::XMFLOAT4X4& GetView() const { return data_.view_; }

	// プロジェクション行列取得
	const DirectX::XMFLOAT4X4& GetProjection() const { return data_.projection_; }

	// 視点取得
	const Vector3& GetEye() const { return data_.eye_; }

	// 注視点取得
	const Vector3& GetFocus() const { return data_.focus_; }

	// 上方向取得
	const Vector3& GetUp() const { return data_.up_; }

	// 前方向取得
	const Vector3& GetFront() const { return data_.front_; }

	// 右方向取得
	const Vector3& GetRight() const { return data_.right_; }

	// データ取得
	const CameraData& GetDate() const { return data_; }
private:
	CameraData data_;
};
