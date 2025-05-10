#pragma once

#include <vector>

#include "../Camera/Camera.h"
#include "RenderState.h"
#include "../Math/Vector.h"
#include "Light.h"

/// <summary>
/// •`‰æ—pî•ñ‚ğŠi”[‚·‚é\‘¢‘Ì
/// </summary>
struct RenderContext
{
	ID3D11DeviceContext* deviceContext = nullptr;
	const RenderState* renderState = nullptr;
	const Camera* camera = nullptr;
	// ƒ‰ƒCƒgî•ñ
	Vector4 lightDirection		= Vector4::Up;
	Vector4 lightColor			= Vector4::White;
	Vector4 lightAmbientColor	= Vector4::White;
	ID3D11ShaderResourceView* const* environmentMap = nullptr;
	std::vector<const PointLight*> pointLights;
};
