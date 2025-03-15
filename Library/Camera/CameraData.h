#pragma once

#include "../Math/Vector.h"

/// <summary>
/// カメラの内部データ
/// </summary>
struct CameraData
{
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