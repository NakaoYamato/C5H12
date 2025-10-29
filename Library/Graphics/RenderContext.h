#pragma once

#include <vector>

#include "../Camera/Camera.h"
#include "RenderState.h"
#include "../Math/Vector.h"
#include "Light.h"

#pragma region 描画関係の定義
const UINT ModelCBIndex = 1; // モデル用定数バッファのインデックス
#pragma endregion


/// <summary>
/// 描画用情報を格納する構造体
/// </summary>
struct RenderContext
{
	ID3D11DeviceContext* deviceContext = nullptr;
	ID3D11DepthStencilView* depthStencilView = nullptr;
	const RenderState* renderState = nullptr;
	const Camera* camera = nullptr;
	// ライト情報
	Vector4 lightDirection		= Vector4::Up;
	Vector4 lightColor			= Vector4::White;
	Vector4 lightAmbientColor	= Vector4::White;
	ID3D11ShaderResourceView* const* environmentMap = nullptr;
	std::vector<const PointLight*> pointLights;
};
