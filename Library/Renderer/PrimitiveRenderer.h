#pragma once

#include <vector>
#include <wrl.h>
#include <d3d11.h>
#include <DirectXMath.h>

#include "../../Library/Math/Vector.h"
#include "../../Library/Math/Quaternion.h"

namespace PrimitiveRenderer
{
	void Initialize(ID3D11Device* device);

	// í∏ì_í«â¡
	void AddVertex(const Vector3& position, const Vector4& color);

	// ï`âÊé¿çs
	void Render(
		ID3D11DeviceContext* dc,
		const DirectX::XMFLOAT4X4& view,
		const DirectX::XMFLOAT4X4& projection);
}
