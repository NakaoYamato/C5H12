#pragma once

#include <vector>

#include "../Camera/CameraData.h"
#include "RenderState.h"
#include "../Math/Vector.h"
#include "../Light/PointLight.h"

/// <summary>
/// •`‰æ—pî•ñ‚ğŠi”[‚·‚é\‘¢‘Ì
/// </summary>
struct RenderContext
{
	ID3D11DeviceContext* deviceContext;
	const RenderState* renderState;
	const CameraData* camera;
	// ƒ‰ƒCƒgî•ñ
	Vector4 lightDirection = { 0.0f,-1.0f,0.0f,0.0f };
	Vector4 lightColor = { 1.0f,1.0f,1.0f,1.0f };
	Vector4 lightAmbientColor = { 1.0f,1.0f,1.0f,1.0f };
	ID3D11ShaderResourceView* const* environmentMap = nullptr;
	std::vector<const PointLight::Data*> pointLights;
};
