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
	ID3D11DeviceContext* deviceContext = nullptr;
	const RenderState* renderState = nullptr;
	const CameraData* camera = nullptr;
	// ƒ‰ƒCƒgî•ñ
	Vector4 lightDirection		= _VECTOR4_UP;
	Vector4 lightColor			= _VECTOR4_WHITE;
	Vector4 lightAmbientColor	= _VECTOR4_WHITE;
	ID3D11ShaderResourceView* const* environmentMap = nullptr;
	std::vector<const PointLight::Data*> pointLights;
};
