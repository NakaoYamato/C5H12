#pragma once

#include "../Math/Vector.h"

/// <summary>
/// カメラの内部データ
/// </summary>
struct CameraData
{
	DirectX::XMFLOAT4X4		view_{};
	DirectX::XMFLOAT4X4		projection_{};

	Vector3		eye_{};
	Vector3		focus_{};

	Vector3		up_{};
	Vector3		front_{};
	Vector3		right_{};

	float fovY_ = 0;
	float aspect_ = 0;
	float nearZ_ = 0;
	float farZ_ = 0;
};